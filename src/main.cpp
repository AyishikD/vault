#define _HAS_STD_BYTE 0
#define WIN32_LEAN_AND_MEAN
#include "vault/vault_ts.hpp"
#include "vault/tcp_server.hpp"
#include "win_soc_init.hpp"
#include <sstream>
#include <iostream>
using namespace std;

WinsockInit ws;  

int main(int argc, char** argv) {
    KVStore core;
    KVStoreThreadSafe store(core);

    if (argc > 1 && std::string(argv[1]) == "--repl") {
        std::string line;
        while (std::getline(std::cin, line)) {
            std::istringstream iss(line);
            std::vector<std::string> parts;
            std::string word;
            while (iss >> word) parts.push_back(word);
        }
    } else {
        TcpServer server(store, 6389);
        server.run();
    }
}
