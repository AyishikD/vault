#pragma once
#include <unordered_map>
#include <string>
#include <set>
#include <utility>
#include "storage.hpp"
#include <shared_mutex>

using namespace std;


class KVStore {
public:
    static const int MAX_VERSIONS = 10;
    KVStore(); 
    void set(const string& key, const string& value);
    pair<bool, string> get(const string& key) const;
    int  commit();                       
    bool checkout(int version);          
    void diff(int v1, int v2) const;   
     std::vector<int> listVersions() const;
    bool deleteVersion(int ver);
    void applyDeltas(unordered_map<string, string>& base, const unordered_map<string, string>& deltas) const;
    unordered_map<string, string> reconstructVersion(int version) const;
    void reset();

private:
    unordered_map<string, string> db_;   
    unordered_map<int, unordered_map<string,string>> versions_;
    int curVersion_ = 0;
    Storage storage_;
    mutable shared_mutex mtx_;
};
