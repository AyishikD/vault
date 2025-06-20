#pragma once
#include <string>
#include <unordered_map>

using namespace std;

class Storage {
public:
    Storage(const string& dataDir = "data");         
    void appendWal(const string& key, const string& val);
    void saveSnapshot(int version,
                      const unordered_map<string,string>& db);

    void restore(unordered_map<string,string>& db,
                 unordered_map<int,unordered_map<string,string>>& versions,
                 int& curVersionOut);
    void clearAll();
    void clearWal();
private:
    string dataDir_;
    string walPath_;
};
