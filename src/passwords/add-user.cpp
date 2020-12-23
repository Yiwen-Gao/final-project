#include "db-manager.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "usage: ./add-user <username> <password>" << endl;
        return 1;
    }

    string username = argv[1];
    string password = argv[2];

    add_user( username, password );
}
