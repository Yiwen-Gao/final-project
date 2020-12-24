#include "conn.h"

#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/err.h>

using namespace std;

const char *CA_CERT = "./trusted_certs/ca-chain.cert.pem";
const char *CLIENT_CERT = "./dummy/cert.pem";
const char *CLIENT_KEY = "./dummy/key.pem";

string format_msgs(vector<string> msgs) {
	string mail = "";
	for (auto it = msgs.begin(); it != msgs.end(); ++it) {
		string m = *it;
		mail += to_string(m.length()) + "\n" + m + "\n";
	}

	return mail;
}

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
	
	SendMsgReq req = SendMsgReq(users);
	conn.send(req.get_http_content());

	string http_content = conn.recv();
	string body = remove_headers(http_content);
	MailCertResp resp = MailCertResp(body);
	
	vector<string> msgs;
	for (auto it = resp.certs.begin(); it != resp.certs.end(); ++it) {
		string cert = *it;
		//write the certificate to a temporary output file called signer.pem
		FILE *fp = fopen("signer.pem", "wb");
		if (fp == NULL)
		{
			cerr << "Failed to open certificate file for writing" << endl;
			return 1;
		}
		char buffer[128];
		int n = 0;
		while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
		{
			if (fwrite(buffer, sizeof(char), n, fp) < 0)
			{
				cerr << "writing to file failed" << endl;
				return 1;
			}

		}
		fclose(fp);		
			
		BIO *in = NULL, *out = NULL, *tbio = NULL;
		X509 *rcert = NULL;
		STACK_OF(X509) *recips = NULL;
		CMS_ContentInfo *cms = NULL;
		int ret = 1;
		// string cert = *it;
		// // TODO encrypt msg
		// BIO *in = NULL, *out = NULL, *tbio = NULL;
		// X509 *rcert = NULL;
		// STACK_OF(X509) *recips = NULL;
		// CMS_ContentInfo *cms = NULL;
		// int ret = 1;
		// int flags = CMS_STREAM;
		
		OpenSSL_add_all_algorithms();
		ERR_load_crypto_strings();

		tbio = BIO_new_file("signer.pem", "r");
		if (!tbio)
			goto err;
		rcert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
		
		if (!rcert)
			goto err;

		recips = sk_X509_new_null();

		if (!recips || !sk_X509_push(recips, rcert))
			goto err;

		//TODO: delete temp file


err:
	if (ret) {
		fprintf(stderr, "Error Encrypting Data\n");
		ERR_print_errors_fp(stderr);	
	}

	CMS_ContentInfo_free(cms);
	X509_free(rcert);
	sk_X509_pop_free(recips, X509_free);
	BIO_free(in);
	BIO_free(out);
	BIO_free(tbio);
	return ret;
		// OpenSSL_add_all_algorithms();
		// ERR_load_crypto_strings();
		// tbio = BIO_new_file("signer.pem", "r");
		// if (!tbio) {
		// 	//goto err;
		// }

		msgs.push_back(msg);
	}

	string mail = format_msgs(msgs);
	conn.send(mail);
	conn.recv();

	return 0;

}

