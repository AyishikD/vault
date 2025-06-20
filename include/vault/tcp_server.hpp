#pragma once
#include <winsock2.h>
#include "vault/vault_ts.hpp"   // thread‑safe wrapper

class TcpServer {
public:
    TcpServer(KVStoreThreadSafe& store, unsigned short port = 6389);
    ~TcpServer();
    void run();                    // blocking accept loop
private:
    void   handleClient(SOCKET);   // per‑connection worker
    SOCKET listenSock_{INVALID_SOCKET};
    unsigned short port_;
    KVStoreThreadSafe& store_;
};
