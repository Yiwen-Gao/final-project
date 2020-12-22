#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <crypt.h>


char * crypt_gensalt(char const *prefix, unsigned long count, char const *rbytes, int nrbytes);

const std::string DB_NAME = "db.csv";

std::string hash_with_salt(const std::string &pw, const std::string &salt)
{
	return crypt(pw.c_str(), salt.c_str());
}

std::string salt_and_hash(const std::string &pw, std::string &salt)
{
	char *tmp = crypt_gensalt(NULL, 0, NULL, 0);
	salt = tmp;
	return hash_with_salt(pw, salt);
}

int verify_pw(const std::string &username, const std::string &pw)
{
	std::string line;
	size_t first_del, second_del;
	std::string name, salt, hash;
	std::ifstream db (DB_NAME);
	if (db.is_open())
	{
		while (std::getline(db, line))
		{
			first_del = line.find(' ');
			second_del = line.find(' ', first_del + 1);
			name = line.substr(0,first_del);
			if (name == username)
			{
				salt = line.substr(first_del+1, second_del);
				hash = line.substr(second_del+1);
			}
		}
		db.close();
	}
	return (hash_with_salt(pw, salt) == hash);
}

int update_user(const std::string &username, const std::string &pw)
{
	std::vector<std::string> lines;
	std::string line, name;
	std::ifstream db (DB_NAME);
	size_t i = 0, first_del;
	size_t ind = -1;
	if (db.is_open())
	{
		while (std::getline(db, line))
		{
			first_del = line.find(' ');
			name = line.substr(0, first_del);
			if (name == username)
			{
				ind = i;
			}
			lines.push_back(line + "\n");
			++i;
		}
		db.close();
	}
	std::string salt, hash;
	hash = salt_and_hash(pw, salt);
	if (ind != -1)
	{
		lines[ind] = username + " " + salt + " " + hash + "\n";
	}
	else
	{
		lines.push_back(username + " " + salt + " " + hash + "\n");
	}
	std::ofstream db_out (DB_NAME);
	if (db_out.is_open())
	{
		for (std::string to_print : lines)
		{
			db_out << to_print;
		}
		db_out.close();
	}
	return 0;

}

int main(int argc, char* argv[])
{
	update_user(argv[1], argv[2]);	
}
