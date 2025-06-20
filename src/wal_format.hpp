#define _NO_CPP_STANDARD_BYTE
#define _HAS_STD_BYTE 0       // <‑‑ disables std::byte in <cstddef>
#define WIN32_LEAN_AND_MEAN   // optional: trims <windows.h>
#pragma once
#include <cstdint>
#include <string_view>


enum class OpCode : uint8_t { SET = 1 };

struct WalRecord {
    OpCode   op;
    uint32_t keyLen;
    uint32_t valLen;
    std::string_view key;
    std::string_view val;
};