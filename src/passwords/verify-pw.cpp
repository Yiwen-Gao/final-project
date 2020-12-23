#include "db-manager.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "usage: ./verify-pw <username> <password>" << endl;
        exit(1);
    }

    string username = argv[1];
    string password = argv[2];
    
    if (verify_user(username, password)) {
        cout << "verify-pw: correct password" << endl;
        return 0;
    } else {
        cout << "verify-pw: incorrect username or password" << endl;
        return 1;
    }
}
