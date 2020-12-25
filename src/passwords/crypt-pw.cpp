#include "db-manager.h"

int main() {
    vector<User> users;
	ifstream file("original-user-info.txt");
	size_t first_del, second_del;
	string line, username, temp, password;
	
	if (file.is_open()) {
		while (getline(file, line)) {
			first_del = line.find(' ');
			second_del = line.find(' ', first_del + 1);
			username = line.substr(0, first_del);
			temp = line.substr(first_del + 1, second_del - first_del);
			password = line.substr(second_del + 1);
			User user = hash_info(username, "", password);
			users.push_back(user);
		}
		file.close();
	} else {
        cerr << "crypt-pw: failed to open initial file" << endl;
        return 1;
    }
	
    if (update_all_users(users)) {
        cerr << "crypt-pw: initialized password database" << endl;
        return 0;
    } else {
        cerr << "crypt-pw: failed to initialize password database" << endl;
        return 1;
    }
}