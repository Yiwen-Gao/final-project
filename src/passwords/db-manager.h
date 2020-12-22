#ifndef DBMANAGER_H
#define DBMANAGER_H

/*************************** HEADER FILES ***************************/
#include <crypt.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

char * crypt_gensalt(char const *prefix, unsigned long count, char const *rbytes, int nrbytes);

/****************************** MACROS ******************************/
#define DB_NAME "passwords.txt"

/*********************** CLASS DECLARATIONS **********************/
class User {
    public:
        string username;
        string salt;
        string hashed_password;

        User(string username, string salt, string hashed_password) {
            this->username = username;
            this->salt = salt;
            this->hashed_password = hashed_password;
        }
};

/*********************** FUNCTION DECLARATIONS **********************/
User hash_info(string username, string salt, string password);

vector<User> select_all_users();
bool update_all_users(vector<User> users);
bool contains_user(string username);
bool verify_user(string username, string password);
bool update_user(string username, string prev_password, string curr_password);
bool updated_all_users(vector<User> users);

#endif
