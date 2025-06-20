#pragma once
#include <winsock2.h>
#include "vault/vault_ts.hpp"   

class TcpServer {
public:
    TcpServer(KVStoreThreadSafe& store, unsigned short port = 6389);
    ~TcpServer();
    void run();                   
private:
    void   handleClient(SOCKET);   
    SOCKET listenSock_{INVALID_SOCKET};
    unsigned short port_;
    KVStoreThreadSafe& store_;
};
