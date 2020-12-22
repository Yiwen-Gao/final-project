#include "db-manager.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "usage: ./verify-pw <username> <password>" << endl;
        exit(1);
    }

    string username = argv[1];
    string password = argv[2];
    
    if (verify_user(username, password)) {
        cerr << "verify-pw: correct password" << endl;
        return 0;
    } else {
        cerr << "verify-pw: incorrect username or password" << endl;
        return 1;
    }
}