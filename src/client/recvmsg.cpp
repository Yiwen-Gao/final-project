#include "conn.h"
#include <fstream>
extern "C" {
    #include <stdio.h>

    #include <openssl/pem.h>
    #include <openssl/cms.h>
    #include <openssl/err.h>
}

using namespace std;

const string CHAIN_CERT = "./trusted_certs/ca-chain.cert.pem";
const string CA_CERT = "./trusted_certs/ca.cert.pem";
const string INT_CERT = "./trusted_certs/intermediate.cert.pem";
const string CLIENT_CERT_PREFIX = "./certificates/";
const string CLIENT_KEY_PREFIX = "./csr/private/";

const string INPUT_PATH = "./smout.txt"; // "./input.txt";
const string DECRYPT_PATH = "./decrypt.txt";
const string VERIFY_PATH = "./verify.txt";
const string CREDENTIALS_PATH = "./credentials.txt";
const string SENDER_CERT = "./sender.cert.pem";

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
        string line;
        while (getline(input_file, line)) {
            msg += line + "\n";
        }
        input_file.close();
    }

    return msg.substr(0, msg.length() - 1);
}

void delete_files(vector<string> filenames) {
    for (auto it = filenames.begin(); it != filenames.end(); ++it) {
        const char *file = (*it).c_str();
        if (remove(file) != 0) {
            cerr << "sendmsg: failed to remove " << *it << endl;
        }
    }
}

bool verify_signature(string ca_cert, string int_cert, string sender_cert, string input, string output) {
    BIO *in = NULL, *out = NULL, *tbio = NULL, *ibio = NULL, *sbio = NULL, *cont = NULL;
    X509_STORE *st = NULL;
    X509 *cacert = NULL, *icert = NULL, *scert = NULL;
    CMS_ContentInfo *cms = NULL;
    STACK_OF(X509) *certs;
    bool is_verified = false;

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    /* Set up trusted CA certificate store */
    st = X509_STORE_new();

    /* Read in CA certificate */
    tbio = BIO_new_file(ca_cert.c_str(), "r");
    if (!tbio)
        goto err;

    cacert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
    if (!cacert)
        goto err;

    if (!X509_STORE_add_cert(st, cacert))
        goto err;

    ibio = BIO_new_file(int_cert.c_str(), "r");
    if (!ibio)
        goto err;

    icert = PEM_read_bio_X509(ibio, NULL, 0, NULL);
    if (!icert)
        goto err;

    if (!X509_STORE_add_cert(st, icert))
        goto err;

    // read in sender cert
    sbio = BIO_new_file(sender_cert.c_str(), "r");
    if (!sbio)
        goto err;

    scert = PEM_read_bio_X509(sbio, NULL, 0, NULL);
    if (!scert)
        goto err;

    if (!X509_STORE_add_cert(st, scert))
        goto err;

    certs = sk_X509_new_null();
    sk_X509_push(certs, scert);

    /* Open message being verified */
    in = BIO_new_file(input.c_str(), "r");
    if (!in)
        goto err;

    /* parse message */
    cms = SMIME_read_CMS(in, &cont);
    if (!cms)
        goto err;

    /* File to output verified content to */
    out = BIO_new_file(output.c_str(), "w");
    if (!out)
        goto err;
    //cout << "let's try to verify" << endl;

    if (!CMS_verify(cms, certs, st, cont, out, CMS_NOINTERN)) {
        fprintf(stderr, "Verification Failure\n");
        goto err;
    }

    fprintf(stderr, "Verification Successful\n");
    is_verified = true;

 err:
    if (!is_verified) {
        fprintf(stderr, "Error Verifying Data\n");
        ERR_print_errors_fp(stderr);
    }

    CMS_ContentInfo_free(cms);
    X509_free(cacert);
    BIO_free(in);
    BIO_free(out);
    BIO_free(tbio);

    sk_X509_free(certs);
    X509_free(scert);
    BIO_free(sbio);
    return is_verified;
}

bool decrypt_msg(string credentials, string input, string output) {
    BIO *in = NULL, *out = NULL, *tbio = NULL;
    X509 *rcert = NULL;
    EVP_PKEY *rkey = NULL;
    CMS_ContentInfo *cms = NULL;
    bool is_decrypted = false;

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    /* Read in recipient certificate and private key */
    tbio = BIO_new_file(credentials.c_str(), "r");
    if (!tbio)
        goto err;
    
    rcert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
    BIO_reset(tbio);
    rkey = PEM_read_bio_PrivateKey(tbio, NULL, 0, NULL);
    if (!rcert || !rkey)
        goto err;

    /* Open S/MIME message to decrypt */
    in = BIO_new_file(input.c_str(), "r");
    if (!in)
        goto err;

    /* Parse message */
    cms = SMIME_read_CMS(in, NULL);
    if (!cms)
        goto err;

    out = BIO_new_file(output.c_str(), "w");
    if (!out)
        goto err;

    /* Decrypt S/MIME message */
    if (!CMS_decrypt(cms, rkey, rcert, NULL, out, 0))
        goto err;

    fprintf(stderr, "Decryption Successful\n");
    is_decrypted = true;

 err:
    if (!is_decrypted) {
        fprintf(stderr, "Error Decrypting Data\n");
        ERR_print_errors_fp(stderr);
    }

    CMS_ContentInfo_free(cms);
    X509_free(rcert);
    EVP_PKEY_free(rkey);
    BIO_free(in);
    BIO_free(out);
    BIO_free(tbio);
    return is_decrypted;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		cerr << "sendmsg: missing username" << endl;
		exit(1);
	}

    string username = argv[1];
    string client_cert_path = CLIENT_CERT_PREFIX + username + ".cert.pem";
    string client_key_path = CLIENT_KEY_PREFIX + username + ".key.pem";
    ClientConnection conn = ClientConnection(CHAIN_CERT.c_str(), client_cert_path.c_str(), client_key_path.c_str());
    conn.connect_server();
    
    // comm with server
    RecvMsgReq req = RecvMsgReq(username);
	conn.send(req.get_http_content());
    int len;
    conn.read_bytes((char *) &len, sizeof(int));
    char *send_cert_ptr = (char *) malloc(len);
    conn.read_bytes(send_cert_ptr, len);
    string send_cert (send_cert_ptr, len);
    free(send_cert_ptr);

    conn.read_bytes((char *) &len, sizeof(int));
    char *body_ptr = (char *) malloc(len);
    conn.read_bytes(body_ptr, len);
    string content (body_ptr, len);
    free(body_ptr);
    int one = content.find("\n");
    if (one == string::npos)
    {
        return -1;
    }
    int two = content.substr(0, one + 1).find("\n");
    if (two == string::npos)
    {
        return -1;
    }
    string address = content.substr(0, one + two + 1);
    string msg = content.substr(one + two + 2);
    
    msg = content.substr(one + two + 3);
    msg = msg.substr(0, msg.size() - 1);
    /*int trim = msg.find("------");
    trim += msg.substr(trim + 6).find("------") + 6;
    msg = msg.substr(0, trim);
    trim = msg.find("MIME");
    trim += msg.substr(trim + 4).find("MIME") + 4;
    trim += msg.substr(trim + 4).find("MIME") + 4;
    msg = msg.substr(trim);*/
    //cout << endl << endl << "address:" << endl << address << endl << "~~~~~~" << endl;
    //cout << "msg:" << endl << msg << endl << "~~~~~~~" << endl;
    //cout << "cert:" << endl << send_cert << endl << "~~~~~~~" << endl;

    // set up decryption and verification
    write_to_file(INPUT_PATH, msg);
    write_to_file(SENDER_CERT, send_cert);

    string sender_cert = "./dummy/cert.pem";
    if (verify_signature(CA_CERT, INT_CERT, SENDER_CERT, INPUT_PATH, VERIFY_PATH) && decrypt_msg(client_cert_path, VERIFY_PATH, DECRYPT_PATH)) {
    //if (decrypt_msg(client_cert_path, INPUT_PATH, DECRYPT_PATH)) {
        // cout << "[mail content]" << endl;
        // cout << resp.address << endl << endl;
        //cout << read_from_file(DECRYPT_PATH) << endl;
    } else {
        cerr << "sendmsg: failed to decrypt or verify mail" << endl;
    }

    vector<string> intermediates{ CREDENTIALS_PATH, INPUT_PATH, SENDER_CERT, DECRYPT_PATH, VERIFY_PATH };
    // delete_files(intermediates);
	return 0;
}
