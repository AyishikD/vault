#define _HAS_STD_BYTE 0
#define WIN32_LEAN_AND_MEAN
#include "vault/tcp_server.hpp"
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <vector>
#include <string>


static std::string simple(const std::string& s){ return "+"+s+"\r\n"; }
static std::string bulk  (const std::string& s){ return "$"+std::to_string(s.size())+"\r\n"+s+"\r\n"; }
static std::string integer(long long n)        { return ":"+std::to_string(n)+"\r\n"; }
static std::string error( const std::string& s){ return "-ERR "+s+"\r\n"; }



static bool parseRESP(const std::string&, std::vector<std::string>&);


TcpServer::TcpServer(KVStoreThreadSafe& st, unsigned short p)
    : store_(st), port_(p)
{
    listenSock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock_ == INVALID_SOCKET)
        throw std::runtime_error("socket() failed");

    u_long opt = 1;
    setsockopt(listenSock_, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // 0.0.0.0
    addr.sin_port        = htons(port_);

    if (bind(listenSock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
        throw std::runtime_error("bind() failed");

    if (listen(listenSock_, SOMAXCONN) == SOCKET_ERROR)
        throw std::runtime_error("listen() failed");

    std::cout << "KVStore listening on port " << port_ << std::endl;
}

TcpServer::~TcpServer() {
    if (listenSock_ != INVALID_SOCKET) closesocket(listenSock_);
}

void TcpServer::run() {
    while (true) {
        SOCKET cli = accept(listenSock_, nullptr, nullptr);
        if (cli == INVALID_SOCKET) continue;
        std::thread(&TcpServer::handleClient, this, cli).detach();
    }
}

void TcpServer::handleClient(SOCKET s) {
    std::string buf;  char tmp[4096];

    while (int n = recv(s, tmp, 4096, 0)) {
        if (n <= 0) break;
        buf.append(tmp, n);

        std::vector<std::string> parts;
        if (!parseRESP(buf, parts)) continue;   
        buf.clear();

        std::string cmd = parts[0];
        for (auto& c : cmd) c = toupper(c);
        std::string rep;

        if (cmd=="PING") rep = simple("PONG");
        else if (cmd=="SET" && parts.size()==3)          { store_.set(parts[1], parts[2]); rep = simple("OK"); }
        else if (cmd=="GET" && parts.size()==2)          { auto [ok,v]=store_.get(parts[1]); rep = ok?bulk(v):"$-1\r\n"; }
        else if (cmd=="COMMIT")                          { rep = integer(store_.commit()); }
        else if (cmd=="CHECKOUT" && parts.size()==2)     { rep = integer(store_.checkout(std::stoi(parts[1]))?1:0); }
        else if (cmd=="DELETE_VERSION" && parts.size()==2){ rep = integer(store_.deleteVersion(std::stoi(parts[1]))?1:0); }
        else if (cmd=="VERSIONS") {
            auto v = store_.listVersions();
            rep = "*" + std::to_string(v.size()) + "\r\n";
            for (int id : v) rep += bulk(std::to_string(id));
        }
        else if (cmd=="RESET")                           { store_.reset(); rep = simple("OK"); }
        else if (cmd=="QUIT")                            { rep = simple("BYE"); send(s,rep.c_str(),rep.size(),0); break; }
        else                                             { rep = error("unknown cmd"); }

        send(s, rep.c_str(), static_cast<int>(rep.size()), 0);
    }
    closesocket(s);
}

static bool parseRESP(const std::string& in, std::vector<std::string>& out) {
    if (in.empty() || in[0] != '*') return false;
    size_t pos = in.find("\r\n"); if (pos == std::string::npos) return false;
    int parts = std::stoi(in.substr(1, pos - 1));
    size_t idx = pos + 2; out.clear();
    for (int i = 0; i < parts; ++i) {
        if (in[idx] != '$') return false;
        size_t eol = in.find("\r\n", idx); if (eol == std::string::npos) return false;
        int len = std::stoi(in.substr(idx + 1, eol - idx - 1));
        idx = eol + 2; if (idx + len + 2 > in.size()) return false;
        out.emplace_back(in.substr(idx, len));
        idx += len + 2;
    }
    return true;
}