#define _NO_CPP_STANDARD_BYTE
#define _HAS_STD_BYTE 0       // <‑‑ disables std::byte in <cstddef>
#define WIN32_LEAN_AND_MEAN   // optional: trims <windows.h>
#include "vault/storage.hpp"
#include "wal_format.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

Storage::Storage(const string& dir) : dataDir_(dir), walPath_(dir + "/wal.log") {
    fs::create_directories(dataDir_);
}

void Storage::appendWal(const std::string& key, const std::string& value) {
    std::ofstream walFile(dataDir_ + "/wal.log", std::ios::app);
    walFile << key << "=" << value << '\n';
    walFile.flush();  
}


void Storage::saveSnapshot(int ver,
        const unordered_map<string,string>& db) {
    string snap = dataDir_ + "/snap-" + to_string(ver) + ".bin";
    ofstream out(snap, ios::binary | ios::trunc);
    uint32_t count = db.size();
    out.write(reinterpret_cast<char*>(&count), sizeof(count));
    for (auto& [k,v] : db) {
        uint32_t kl = k.size(), vl = v.size();
        out.write(reinterpret_cast<char*>(&kl), 4);
        out.write(reinterpret_cast<char*>(&vl), 4);
        out.write(k.data(), kl);
        out.write(v.data(), vl);
    }
}

void Storage::restore(unordered_map<string,string>& db,
                     unordered_map<int,unordered_map<string,string>>& versions,
                     int& curVersionOut) {
    db.clear();
    versions.clear();
    curVersionOut = 0;
    int latestVersion = 0;
    string latestSnapshot;
    for (const auto& entry : fs::directory_iterator(dataDir_)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (filename.find("snap-") == 0 && filename.find(".bin") != string::npos) {
                int version = stoi(filename.substr(5, filename.find(".bin") - 5));
                if (version > latestVersion) {
                    latestVersion = version;
                    latestSnapshot = entry.path().string();
                }
            }
        }
    }
    
    if (!latestSnapshot.empty()) {
        ifstream in(latestSnapshot, ios::binary);
        if (in) {
            uint32_t count;
            in.read(reinterpret_cast<char*>(&count), sizeof(count));
            
            for (uint32_t i = 0; i < count; i++) {
                uint32_t kl, vl;
                in.read(reinterpret_cast<char*>(&kl), 4);
                in.read(reinterpret_cast<char*>(&vl), 4);
                
                string key(kl, '\0');
                string val(vl, '\0');
                
                in.read(&key[0], kl);
                in.read(&val[0], vl);
                
                db[key] = val;
            }
            
            curVersionOut = latestVersion;
            versions[curVersionOut] = db;
        }
    }
    
    if (fs::exists(walPath_)) {
        ifstream in(walPath_, ios::binary);
        if (in) {
            while (in) {
                WalRecord header;
                if (!in.read(reinterpret_cast<char*>(&header), sizeof(WalRecord) - 2 * sizeof(std::string_view))) {
                    break;
                }
                
                string key(header.keyLen, '\0');
                string value(header.valLen, '\0');
                
                if (!in.read(&key[0], header.keyLen) || !in.read(&value[0], header.valLen)) {
                    break; 
                }
                
                if (header.op == OpCode::SET) {
                    db[key] = value;
                }
            }
        }
    }
}

void Storage::clearAll() {
    std::filesystem::remove_all(dataDir_);
    std::filesystem::create_directories(dataDir_);
}

void Storage::clearWal() {
    std::ofstream walFile(dataDir_ + "/wal.log", std::ios::trunc); 
}