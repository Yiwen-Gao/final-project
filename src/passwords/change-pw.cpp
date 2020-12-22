#include "db-manager.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "usage: ./change-pw <username> <prev_password> <curr_password>" << endl;
        exit(1);
    }

    string username = argv[1];
    string prev_password = argv[2];
    string curr_password = argv[3];
    
    if (update_user(username, prev_password, curr_password)) {
        cerr << "change-pw: changed password" << endl;
        return 0;
    } else {
        cerr << "change-pw: failed to change password" << endl;
        return 1;
    }   
}