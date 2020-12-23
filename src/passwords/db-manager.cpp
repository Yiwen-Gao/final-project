#include "db-manager.h"

User hash_info(string username, string salt, string password) {
	if (salt == "") {
		salt = crypt_gensalt(NULL, 0, NULL, 0);
	}

	string hashed_password = crypt(password.c_str(), salt.c_str());
	return User(username, salt, hashed_password);
}

vector<User> select_all_users() {
	vector<User> users;
	ifstream db(DB_NAME);
	size_t first_del, second_del;
	string line, username, salt, hashed_password;
	
	if (db.is_open()) {
		while (getline(db, line)) {
			first_del = line.find(' ');
			second_del = line.find(' ', first_del + 1);
			username = line.substr(0, first_del);
			salt = line.substr(first_del + 1, second_del - first_del - 1);
			hashed_password = line.substr(second_del + 1);
			User user = User(username, salt, hashed_password);
			users.push_back(user);
		}
		db.close();
	}
	return users;
}

bool contains_user(string username) {
	vector<User> users = select_all_users();
	for (auto it = users.begin(); it != users.end(); ++it) {
		User user = *it;
		if (username == user.username) {
			return true;
		}
	}

	return false;
}

bool verify_user(string username, string password) {
	if (!contains_user(username)) {
		return false;
	}

	vector<User> users = select_all_users();
	for (auto it = users.begin(); it != users.end(); ++it) {
		User prev = *it;
		if (username == prev.username) {
			User curr = hash_info(username, prev.salt, password);
			if (prev.hashed_password == curr.hashed_password) {
				return true;
			} 
		}
	}

	return false;
}

bool update_user(string username, string prev_password, string curr_password) {
	if (!verify_user(username, prev_password)) {
		return false;
	}

	vector<User> users = select_all_users();
	for (auto it = users.begin(); it != users.end(); ++it) {
		User &user = *it;
		if (username == user.username) {
			User temp = hash_info(username, "", curr_password);
			user.salt = temp.salt;
			user.hashed_password = temp.hashed_password;
		}
	}

	return update_all_users(users);
}

bool add_user(string username, string password)
{
    vector<User> users = select_all_users();
    User user = hash_info(username, "", password);
    users.push_back(user);
    
    return update_all_users(users);
}

bool update_all_users(vector<User> users) {
	ofstream db(DB_NAME);
	if (db.is_open()) {
		for (auto it = users.begin(); it != users.end(); ++it) {
			User user = *it;
			db << user.username << " " << user.salt << " " << user.hashed_password << endl;
		}
		db.close();
		return true;
	}

	return false;
}
