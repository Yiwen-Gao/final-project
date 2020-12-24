#include "conn.h"
#include <fstream>
extern "C" {
    #include <openssl/pem.h>
    #include <openssl/cms.h>
    #include <openssl/err.h>
}

using namespace std;

const char *CA_CERT = "./trusted_certs/ca-chain.cert.pem";
const char *CLIENT_CERT = "./dummy/cert.pem";
const char *CLIENT_KEY = "./dummy/key.pem";

const string input_path = "./";
const string output_path = "";

int decrypt() {
    BIO *in = NULL, *out = NULL, *tbio = NULL;
    X509 *rcert = NULL;
    EVP_PKEY *rkey = NULL;
    CMS_ContentInfo *cms = NULL;
    int ret = 1;

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    /* Read in recipient certificate and private key */
    tbio = BIO_new_file(CLIENT_KEY, "r");

    if (!tbio)
        goto err;

    rcert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
    BIO_reset(tbio);
    rkey = PEM_read_bio_PrivateKey(tbio, NULL, 0, NULL);

    if (!rcert || !rkey)
        goto err;

    /* Open S/MIME message to decrypt */
    in = BIO_new_file(input_path.c_str(), "r");
    if (!in)
        goto err;

    /* Parse message */
    cms = SMIME_read_CMS(in, NULL);
    if (!cms)
        goto err;

    out = BIO_new_file(output_path.c_str(), "w");
    if (!out)
        goto err;

    /* Decrypt S/MIME message */
    if (!CMS_decrypt(cms, rkey, rcert, NULL, out, 0))
        goto err;

    ret = 0;

 err:
    if (ret) {
        fprintf(stderr, "Error Decrypting Data\n");
        ERR_print_errors_fp(stderr);
    }

    CMS_ContentInfo_free(cms);
    X509_free(rcert);
    EVP_PKEY_free(rkey);
    BIO_free(in);
    BIO_free(out);
    BIO_free(tbio);
    return ret;
}

void write_to_file(string path, string text) {
    ofstream output_file(path);
    if (output_file.is_open()) {
        output_file << text;
        output_file.close();
    }
}

string read_from_file(string path) {
    string msg = "";
    ifstream input_file(path);
    if (input_file.is_open()) {
        input_file >> msg;
    }

    return msg;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		cerr << "sendmsg: missing username" << endl;
		exit(1);
	}

    string username = argv[1];
    ClientConnection conn = ClientConnection(CA_CERT, CLIENT_CERT, CLIENT_KEY);
    conn.connect_server();
    
    // comm with server
    RecvMsgReq req = RecvMsgReq(username);
	conn.send(req.get_http_content());
    string http_content = conn.recv();
    string body = remove_headers(http_content);
    MailResp resp = MailResp(body);

    // output mail
    cout << "[mail content]" << endl;
    cout << resp.address << endl << endl;
    
    write_to_file(input_path, resp.msg);
    cout << read_from_file(output_path);

	return 0;
}