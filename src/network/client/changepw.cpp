#include "conn.h"

using namespace std;

const string CHANGE_PW_HEADER = "POST changepw HTTP/1.0\n";

string createPwReq(string username, string prev_password, string curr_password) {
	string content = username + "\n" + prev_password + "\n" + curr_password + "\n";
	string pwReq = CHANGE_PW_HEADER; 
	pwReq += "Content-Length: " + to_string(content.size()) + "\n\n";
	pwReq += content;

	return pwReq;
}

string parseCertResp(string resp) {

}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		cerr << "Must supply a username" << endl;
		exit(1);
	}

	string username = argv[1];
	string prev_password = getpass("Enter password");
	string curr_password = getpass("Enter new password");
	
	ClientConnection conn = ClientConnection("", "", "");
	string pwReq = createPwReq(username, prev_password, curr_password);
	conn.send(pwReq);
	string resp = conn.recv();
	string cert = parseCertResp(resp);

	//open a file descriptor for writing the certificate to the certificates file
	string certificateFile = "./certificates/" + username + ".cert";

	FILE *certfp = fopen(certificateFile.c_str(), "wb");
	if (certfp == NULL) {
		cerr << "Unable to write to certificate file" << endl;
		return 0;
	}

	while ((ilen = SSL_read(ssl, ibuf, sizeof ibuf - 1)) > 0) {
		ibuf[ilen] = '\0';
		if (fwrite(ibuf, sizeof(char), ilen, certfp) < 0)
		{
			cerr << "Writing to file failed" << endl;
			return 0;
		}
		printf("%s", ibuf);
	}

	return 0;
}
