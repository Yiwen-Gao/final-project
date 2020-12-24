#include "conn.h"

using namespace std;

const char *CA_CERT = "./trusted_certs/ca-chain.cert.pem";
const char *CLIENT_CERT = "./dummy/cert.pem";
const char *CLIENT_KEY = "./dummy/key.pem";

int main(int argc, char *argv[]) {
	if (argc != 2) {
		cerr << "sendmsg: missing input file" << endl;
		exit(1);
	}

	cout << "separate users by newlines; separate users from message by two newlines; terminate msg with a period and a newline" << endl;
	string username = argv[1];
	vector<string> users;
	string line, msg;
	bool is_recpts = true;
	while (getline(cin, line)) {
		if (line == ".") {
			break;
		} else if (is_recpts && line == "") {
			is_recpts = false;
		} else if (is_recpts) {
			users.push_back(line);
		} else {
			msg += line + "\n";
		}
	}

	ClientConnection conn = ClientConnection(CA_CERT, CLIENT_CERT, CLIENT_KEY);
	conn.connect_server();
	
	SendMsgUsersReq users_req = SendMsgUsersReq(users);
	conn.send(users_req.get_http_content());

	string http_content = conn.recv();
	MailCertResp resp = MailCertResp(http_content);
	
	// vector<string> msgs;
	// for (auto it = resp.certs.begin(); it != resp.certs.end(); ++it) {
	// 	string cert = *it;
	// 	// TODO encrypt msg
	// 	msgs.push_back(msg);
	// }
	// SendMsgMailReq mail_req = SendMsgMailReq(msgs);
	// conn.send(mail_req.get_http_content());

	return 0;
}

