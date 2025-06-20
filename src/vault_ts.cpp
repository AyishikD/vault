#define _HAS_STD_BYTE 0          
#define WIN32_LEAN_AND_MEAN

#include "vault/vault_ts.hpp"
#include <shared_mutex>

int KVStoreThreadSafe::set(const std::string& k,const std::string& v){
    std::unique_lock lock(m_);
    core_.set(k,v);
    return 0;
}
std::pair<bool, std::string> KVStoreThreadSafe::get(const std::string& k) const {
    std::shared_lock lock(m_);
    return core_.get(k);
}
int KVStoreThreadSafe::commit(){
    std::unique_lock lock(m_);
    return core_.commit();
}
bool KVStoreThreadSafe::checkout(int v){
    std::unique_lock lock(m_);
    return core_.checkout(v);
}
void KVStoreThreadSafe::diff(int a,int b) const{
    std::shared_lock lock(m_);
    core_.diff(a,b);
}
std::vector<int> KVStoreThreadSafe::listVersions() const {
    std::shared_lock lock(m_);
    return core_.listVersions();  
}

bool KVStoreThreadSafe::deleteVersion(int v){
    std::unique_lock lock(m_);
    return core_.deleteVersion(v);
}
void KVStoreThreadSafe::reset(){
    std::unique_lock lock(m_);
    core_.reset();
}