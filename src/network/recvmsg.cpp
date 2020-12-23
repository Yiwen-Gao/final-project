#include "conn.h"

using namespace std;

const char *CA_CERT = "";
const char *CLIENT_CERT = "";
const char *CLIENT_KEY = "";
const string RECV_MSG_HEADER = "POST recvmsg HTTP/1.0\n"; 

int main(int argc, char *argv[]) {
	if (argc != 2) {
		cerr << "sendmsg: missing username" << endl;
		exit(1);
	}

    string username = argv[1];
    ClientConnection conn = ClientConnection(CA_CERT, CLIENT_CERT, CLIENT_KEY);

	string mailReq = RECV_MSG_HEADER;
    mailReq += "Content-Length: " + to_string(username.length()) + "\n";
    mailReq += username + "\n\n";
	conn.send(mailReq);

    string resp = conn.recv();
    istringstream msg(resp);
    string line;
    int num = 0;
    string enc_mail = "";

    while (getline(msg, line)) {
        if (num <= 1) {
            num += 1;
        } 
        enc_mail += line;
    }

    // remove last two newlines 
    enc_mail = enc_mail.substr(0, enc_mail.length() - 2);
    dec_mail = decrypt();
    cout << dec_mail << endl;

	return 0;
}