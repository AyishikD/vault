#define _NO_CPP_STANDARD_BYTE
#define _HAS_STD_BYTE 0       
#define WIN32_LEAN_AND_MEAN   
#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct WinsockInit {
    WinsockInit()  { WSADATA w; WSAStartup(MAKEWORD(2,2), &w); }
    ~WinsockInit() { WSACleanup(); }
};