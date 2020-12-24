#include "conn.h"

using namespace std;

const char *CA_CERT = "./trusted_certs/ca-chain.cert.pem";
const char *CLIENT_CERT = "../../../server/certificates/ca/intermediate/certs/localhost.cert.pem";
const char *CLIENT_KEY = "../../../server/certificates/ca/intermediate/private/localhost.key.pem";
const string SEND_MSG_HEADER = "POST sendmsg HTTP/1.0\n"; 

int main(int argc, char *argv[]) {
	if (argc != 2) {
		cerr << "sendmsg: missing input file" << endl;
		exit(1);
	}

	// TODO read from file
	string username = argv[1];
	vector<string> users = vector<string> {"wamara", "whaledom", "addleness"}; 
	string msg = "hello friends:)";

	ClientConnection conn = ClientConnection(CA_CERT, CLIENT_CERT, CLIENT_KEY);
	conn.connect_server();
	
	SendMsgUsersReq users_req = SendMsgUsersReq(users);
	conn.send(users_req.get_http_content());

	string http_content = conn.recv();
	MailCertResp resp = MailCertResp(http_content);
	
	vector<string> msgs;
	for (auto it = resp.certs.begin(); it != resp.certs.end(); ++it) {
		string cert = *it;
		// TODO encrypt msg
		msgs.push_back(msg);
	}
	SendMsgMailReq mail_req = SendMsgMailReq(msgs);
	conn.send(mail_req.get_http_content());

	return 0;
}

