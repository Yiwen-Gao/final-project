#include "conn.h"

using namespace std;

const char *CA_CERT = "";
const char *CLIENT_CERT = "";
const char *CLIENT_KEY = "";
const string SEND_MSG_HEADER = "POST sendmsg HTTP/1.0\n"; 

string createCertReq(vector<string> users) {
	string content = "";
	for (auto it = users.begin(); it != users.end(); ++it) {
		content += *it + "\n";
	}
	string certReq = SEND_MSG_HEADER;
	certReq += "Content-Length: " + to_string(content.length()) + "\n";
	certReq += content + "\n";
	return certReq;
}

vector<string> parseCertResp(string resp) {
	vector<string> certs;
	istringstream msg(resp);
	string line;
	int num = 0;
	string c = "";
	
	while (getline(msg, line)) {
		if (num <= 1) {
			num += 1;
		} else if (line == "\n") {
			certs.push_back(c);
			c = "";
		} else {
			c += line;
		}
	}

	certs.push_back(c);
	return certs;
}

string createMailReq(vector<string> users, vector<string> certs) {
	string content = "";
	for (int i = 0; i < users.size(); i++) {
		string un = users[i];
		string cert = certs[i];
		content += un + "\n" + cert + "\n";
	}
	string mailReq = SEND_MSG_HEADER;
	mailReq += "Content-Length: " + to_string(content.length()) + "\n";
	mailReq += content + "\n";
	return mailReq;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		cerr << "sendmsg: missing input file" << endl;
		exit(1);
	}

	string username = argv[1];
	vector<string> users = vector<string> {"wamara", "whaledom", "addleness"}; 
	string tempMail = "hello friends:)";

	ClientConnection conn = ClientConnection(CA_CERT, CLIENT_CERT, CLIENT_KEY);
	
	string certReq = createCertReq(users);
	conn.send(certReq);

	string resp = conn.recv();
	vector<string> certs = parseCertResp(resp);

	string mailReq = createMailReq(users, certs);
	conn.send(mailReq);

	return 0;
}
