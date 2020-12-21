#include <string>
#include <iostream>
#include <fstream>
#include <vector>

const std::string DB_NAME = "db.csv";

std::string hash(const std::string &pw, const std::string &salt)
{
	return "test";
}

std::string salt_and_hash(const std::string &pw, std::string &salt)
{
	salt = " ";
	return hash(pw, salt);
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
	return hash(pw, salt) == hash;
}

int update_user(const std::string &username, const std::string &pw)
{
	std::vector<std::string> lines;
	std::string line;
	std::ifstream db (DB_NAME);
	size_t i = 0;
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
		for (line : lines)
		{
			db_out << line;
		}
		db_out.close();
	}

}

int main(int argc, char* argv[])
{
	
}
