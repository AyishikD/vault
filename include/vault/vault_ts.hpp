#pragma once
#include <shared_mutex>
#include <string>
#include <vector>
#include "vault/vault.hpp"          

class KVStoreThreadSafe {
public:
    KVStoreThreadSafe(KVStore& store) : core_(store) {}
    int  set   (const std::string&, const std::string&);
    std::pair<bool,std::string> get(const std::string&) const;
    std::vector<int> listVersions() const;
    int  commit();
    bool checkout(int);
    void diff(int,int) const;
    bool deleteVersion(int);
    void reset();

private:
    KVStore& core_;   
    mutable std::shared_mutex  m_;    
};
