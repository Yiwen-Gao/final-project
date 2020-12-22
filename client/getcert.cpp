#include <iostream>
#include <string>
#include <unistd.h>
#include "clienttools.h"
/* 
 * Gets a certificate from the server
 * 1. csr should already be generated
 * 2. sends the username and password to the server 
 * 3. send the csr to the server
 * 4. receives the certificate from the server
 * 5. stores the server locally
 */

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		cerr << "Must supply a username" << endl;
		exit(1);
	}

	string username = argv[1];

	string password = getpass("Enter password");


	//assuming csr already generated, if not, might be a doozy.

}
