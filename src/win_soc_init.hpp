#define _NO_CPP_STANDARD_BYTE
#define _HAS_STD_BYTE 0       // <‑‑ disables std::byte in <cstddef>
#define WIN32_LEAN_AND_MEAN   // optional: trims <windows.h>
#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct WinsockInit {
    WinsockInit()  { WSADATA w; WSAStartup(MAKEWORD(2,2), &w); }
    ~WinsockInit() { WSACleanup(); }
};