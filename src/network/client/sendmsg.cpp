#include "conn.h"
#include <fstream>
#include <sstream>

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
		cerr << "sendmsg: missing username" << endl;
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
		if (fwrite(cert.c_str(), sizeof(char), cert.length(), fp) < 0)
		{
			cerr << "writing to file failed" << endl;
			return 1;
		}

		fclose(fp);		
		
		//create temporary file containing the message to be encrypted
		FILE *encfp = fopen("encr.txt", "wb");
		if (encfp == NULL)
		{
			cerr << "Failed to open file for writing" << endl;
			return 1;
		}
		if (fwrite(msg.c_str(), sizeof(char), msg.length(), encfp) < 0)
		{
			cerr << "writing to file failed" << endl;
			return 1;
		}

		fclose(encfp);			

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
		rcert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
		
		if (!rcert)
			goto err;

		recips = sk_X509_new_null();

		if (!recips || !sk_X509_push(recips, rcert))
			goto err;

		rcert = NULL;

		in = BIO_new_file("encr.txt", "r");

		if (!in)
			goto err;

		cms = CMS_encrypt(recips, in, EVP_des_ede3_cbc(), flags);
		if (!cms)
			goto err;
		
		out = BIO_new_file("smencr.txt", "w");
		if (!out)
			goto err;

		if (!SMIME_write_CMS(out, cms, in, flags))
			goto err;
		
		ret = 0;		

		
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

		BIO *ins = NULL, *outs = NULL, *tbios = NULL;
		X509 *scert = NULL;
		EVP_PKEY *skey = NULL;
		CMS_ContentInfo *scms = NULL;
		int sret = 1;

		int flagss = CMS_DETACHED | CMS_STREAM;
	
		string signfilename = "./certificates/" + username + ".cert.pem";
		tbios = BIO_new_file(signfilename.c_str(), "r");

		if (!tbios)
			goto err2;

		scert = PEM_read_bio_X509(tbios, NULL, 0, NULL);

		BIO_reset(tbios);
		
		skey = PEM_read_bio_PrivateKey(tbios, NULL, 0, NULL);

		if (!scert || !skey)
		{
			goto err2;
		}

		ins = BIO_new_file("smencr.txt", "r");
		if (!ins)
		{
			goto err2;
		}

		scms = CMS_sign(scert, skey, NULL, ins, flagss);
		if (!scms)
			goto err2;

		outs = BIO_new_file("smout.txt", "w");

		if (!outs)
			goto err2;
		
		if (!(flags & CMS_STREAM))
			BIO_reset(ins);

		if (!SMIME_write_CMS(outs, scms, ins, flagss))
			goto err2;
	
		sret = 0;
err2:
		if (sret) {
			fprintf(stderr, "Error Signing Data\n");
			ERR_print_errors_fp(stderr);
		}

		CMS_ContentInfo_free(scms);
		X509_free(scert);
		EVP_PKEY_free(skey);
		BIO_free(ins);
		BIO_free(outs);
		BIO_free(tbios);
	
		std::ifstream encryptedandsigned;
		encryptedandsigned.open("smout.txt");
		
		std::stringstream strstream;
		strstream << encryptedandsigned.rdbuf();
		msg = strstream.str();
		//move these to the very end.
		if (remove("encr.txt") != 0)
		{
			cerr << "Problem removing temp file" << endl;
		}

		if (remove("signer.pem") != 0)
		{
			cerr << "Problem removing temp file" << endl;
		}

		if (remove("smout.txt") != 0)
		{
			cerr << "Problem removing temp file" << endl;
		}

		if (remove("smencr") != 0)
		{
			cerr << "Problem removing temp file" << endl;
		}

		//set msg = to the encrypted text read from the file
		msgs.push_back(msg);
	}

	string mail = format_msgs(msgs);
	conn.send(mail);
	conn.recv();

	return 0;

}

