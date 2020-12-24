#include <string>
#include <iostream>
#include "mail_utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


using namespace std;
int main (int argc, char **argv)
{
	if (argc != 2)
	{
		cerr << "Must supply recipient as command line argument" << endl;
		return 1;
	}
	std::string mailbox_name = argv[1];
	if (!validMailboxChars(mailbox_name) || mailbox_name.length() > MAILBOX_NAME_MAX || !doesMailboxExist(mailbox_name)){
		return 1;
	}
	
	string next_file_name = getCurrNumber(mailbox_name);
	if (next_file_name == "ERROR" || next_file_name == "00000")
	{
		return 1;
	}

	std::string new_mail_path = newMailPath(mailbox_name, next_file_name);
	FILE *fp = fopen(new_mail_path.c_str(), "rb");
	if (fp == NULL)
	{
		cerr << "Opening mail failed" << endl;
		return 1;
	}

	int n = 0;
	char buffer[40];
	memset(buffer, 0, 40);
	int contentlength = 0;
	string filecontents = "";
	while ((n = fread(buffer, 1, sizeof(buffer) - 1, fp)) > 0)
	{
		contentlength += n;
		filecontents += buffer;
		memset(buffer, 0, 40);
	}	
	fclose(fp);
	if (remove(new_mail_path.c_str()) != 0)
	{
		cerr << "Problem deleting mail" << endl;
	}
	
	write(STDOUT_FILENO, &contentlength, sizeof(int));
	cout << filecontents << endl;
}
