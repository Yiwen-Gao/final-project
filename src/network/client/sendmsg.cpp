#include "conn.h"

#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/err.h>

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
		BIO *in = NULL, *out = NULL, *tbio = NULL;
		X509 *rcert = NULL;
		STACK_OF(X509) *recips = NULL;
		CMS_ContentInfo *cms = NULL;
		int ret = 1;

		int flags = CMS_STREAM;
		
		OpenSSL_add_all_algorithms();
		ERR_load_crypto_strings();

		tbio = BIO_new_file("signer.pem", "r");
		if (!tbio)
			goto err;


		msgs.push_back(msg);
	}
	SendMsgMailReq mail_req = SendMsgMailReq(msgs);
	conn.send(mail_req.get_http_content());

	return 0;
}

