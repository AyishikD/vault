cmake_minimum_required(VERSION 3.16)
project(vault VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(vault 
    src/vault.cpp
    src/main.cpp
    src/tcp_server.cpp
    src/storage.cpp
    src/win_soc_init.hpp
    src/vault_ts.cpp
)
target_link_libraries(vault PRIVATE ws2_32)