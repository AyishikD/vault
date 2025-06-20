#define _HAS_STD_BYTE 0       
#define WIN32_LEAN_AND_MEAN  
#include "vault/vault.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    KVStore store;
    string cmd;

    cout << "KVStore REPL (SET/GET/COMMIT/CHECKOUT/DIFF/VERSIONS/EXIT)" << endl;
    while (cout << "> " << flush, cin >> cmd) {
        for (auto& ch : cmd) ch = toupper(ch);     
        if (cmd == "SET") {
            string k, v;
            cin >> k;
            getline(cin, v);            
            if (!v.empty() && v[0] == ' ') v.erase(0,1);
            store.set(k, v);
        } else if (cmd == "GET") {
            string k; cin >> k;
            auto [ok, val] = store.get(k);
            cout << (ok ? val : "(nil)") << '\n';
        } else if (cmd == "COMMIT") {
            store.commit();
        } else if (cmd == "CHECKOUT") {
            int ver; cin >> ver;
            if (!store.checkout(ver))
                cout << "Version not found\n";
        } else if (cmd == "DIFF") {
            int v1, v2; cin >> v1 >> v2;
            store.diff(v1, v2);
        } else if (cmd == "VERSIONS") {
            store.listVersions();
        } else if (cmd == "EXIT" || cmd == "QUIT") {
            break;
        } else if (cmd == "DELETE_VERSION") {
            int ver;
            if (cin >> ver) {
                if (!store.deleteVersion(ver))
                    cout << "Version not found\n";
            } else {
                cout << "Usage: DELETE_VERSION <version>\n";
            }
        } else if (cmd == "RESET") {
            store.reset();
        } else {
            cout << "Unknown command\n";
            string rest; getline(cin, rest);
        }
    }
    return 0;
}