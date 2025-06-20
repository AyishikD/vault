#define _NO_CPP_STANDARD_BYTE
#define _HAS_STD_BYTE 0       // <‑‑ disables std::byte in <cstddef>
#define WIN32_LEAN_AND_MEAN   // optional: trims <windows.h>
#include "vault/vault.hpp"
#include <iostream>
#include <climits>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <set>

using namespace std;

KVStore::KVStore() : storage_("data") {
    storage_.restore(db_, versions_, curVersion_);
}

void KVStore::set(const string& key, const string& value) {
    db_[key] = value;
    storage_.appendWal(key, value);
}

pair<bool, string> KVStore::get(const string& key) const {
    auto it = db_.find(key);
    return {it != db_.end(), it == db_.end() ? "" : it->second};
}

void KVStore::applyDeltas(unordered_map<string, string>& base,
                          const unordered_map<string, string>& deltas) const {
    for (const auto& [key, value] : deltas) {
        if (value.empty()) {
            base.erase(key);
        } else {
            base[key] = value;
        }
    }
}

unordered_map<string, string> KVStore::reconstructVersion(int version) const {
    if (version <= 0 || versions_.find(version) == versions_.end()) {
        return {};
    }

    unordered_map<string, string> result;

    vector<int> versionNumbers;
    for (const auto& [ver, _] : versions_) {
        if (ver <= version) {
            versionNumbers.push_back(ver);
        }
    }

    sort(versionNumbers.begin(), versionNumbers.end());

    for (int ver : versionNumbers) {
        const auto& deltas = versions_.at(ver);
        applyDeltas(result, deltas);
    }

    return result;
}

int KVStore::commit() {
    int ver = ++curVersion_;
    if (versions_.size() >= MAX_VERSIONS) {
        int oldestVer = INT_MAX;
        for (const auto& [v, _] : versions_) {
            oldestVer = min(oldestVer, v);
        }
        if (oldestVer < ver) {
            versions_.erase(oldestVer);
        }
        storage_.clearWal();
    }

    unordered_map<string, string> deltas;

    if (versions_.empty()) {
        for (const auto& [key, value] : db_) {
            deltas[key] = value;
        }
    } else {
        auto prevVersion = reconstructVersion(ver - 1);
        for (const auto& [key, value] : db_) {
            auto it = prevVersion.find(key);
            if (it == prevVersion.end() || it->second != value) {
                deltas[key] = value;
            }
        }

        for (const auto& [key, _] : prevVersion) {
            if (db_.find(key) == db_.end()) {
                deltas[key] = "";
            }
        }
    }

    versions_[ver] = deltas;
    storage_.saveSnapshot(ver, db_);
    std::cout << "Committed version " << ver << '\n';
    return ver;
}

bool KVStore::checkout(int ver) {
    auto it = versions_.find(ver);
    if (it == versions_.end()) return false;
    db_ = reconstructVersion(ver);
    std::cout << "Checked out version " << ver << '\n';
    return true;
}

void KVStore::diff(int v1, int v2) const {
    auto it1 = versions_.find(v1);
    auto it2 = versions_.find(v2);
    if (it1 == versions_.end() || it2 == versions_.end()) {
        std::cout << "One or both versions not found\n";
        return;
    }
    const auto a = reconstructVersion(v1);
    const auto b = reconstructVersion(v2);

    std::set<std::string> keys;
    for (const auto& [k, _] : a) keys.insert(k);
    for (const auto& [k, _] : b) keys.insert(k);

    std::cout << "Diff " << v1 << " → " << v2 << ":\n";
    for (const auto& k : keys) {
        auto ia = a.find(k);
        auto ib = b.find(k);
        if (ia == a.end()) {
            std::cout << "  + " << k << " = " << ib->second << '\n';
        } else if (ib == b.end()) {
            std::cout << "  - " << k << " (was " << ia->second << ")\n";
        } else if (ia->second != ib->second) {
            std::cout << "  * " << k << " : " << ia->second << " -> " << ib->second << '\n';
        }
    }
}

std::vector<int> KVStore::listVersions() const {
    std::vector<int> out;
    for (auto& [ver,_] : versions_) out.push_back(ver);
    return out;
}

bool KVStore::deleteVersion(int ver) {
    auto it = versions_.find(ver);
    if (it == versions_.end()) {
        std::cout << "Version " << ver << " not found\n";
        return false;
    }
    versions_.erase(it);
    std::cout << "Deleted version " << ver << '\n';
    return true;
}

void KVStore::reset() {
    db_.clear();
    versions_.clear();
    curVersion_ = 0;
    storage_.clearAll();
    std::cout << "KVStore reset. All data cleared.\n";
}
