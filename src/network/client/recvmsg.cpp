#include "conn.h"
#include <fstream>
extern "C" {
    #include <stdio.h>

    #include <openssl/pem.h>
    #include <openssl/cms.h>
    #include <openssl/err.h>
}

using namespace std;

const string CA_CERT = "./trusted_certs/ca-chain.cert.pem";
const string CLIENT_CERT_PREFIX = "./certificates/";
const string CLIENT_KEY_PREFIX = "./csr/private/";

const string INPUT_PATH = "./smout.txt"; // "./input.txt";
const string DECRYPT_PATH = "./decrypt.txt";
const string VERIFY_PATH = "./verify.txt";
const string CREDENTIALS_PATH = "./credentials.txt";

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

bool verify_signature(string ca_cert, string sender_cert, string input, string output) {
    BIO *in = NULL, *out = NULL, *tbio = NULL, *sbio = NULL, *cont = NULL;
    X509_STORE *st = NULL;
    X509 *cacert = NULL, *scert = NULL;
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
    ClientConnection conn = ClientConnection(CA_CERT.c_str(), client_cert_path.c_str(), client_key_path.c_str());
    conn.connect_server();
    
    // comm with server
    RecvMsgReq req = RecvMsgReq(username);
	conn.send(req.get_http_content());
    string http_content = conn.recv();
    string body = remove_headers(http_content);
    MailResp resp = MailResp(body);

    // set up decryption and verification
    write_to_file(INPUT_PATH, resp.msg);
    string cert = read_from_file(client_cert_path);
    string key = read_from_file(client_key_path);
    write_to_file(CREDENTIALS_PATH, cert + key);

    string sender_cert = "./dummy/cert.pem";
    // if (verify_signature(CA_CERT, sender_cert, INPUT_PATH, VERIFY_PATH) && decrypt_msg(CREDENTIALS_PATH, VERIFY_PATH, DECRYPT_PATH)) {
    if (decrypt_msg(CREDENTIALS_PATH, INPUT_PATH, DECRYPT_PATH)) {
        // cout << "[mail content]" << endl;
        // cout << resp.address << endl << endl;
        cout << read_from_file(DECRYPT_PATH);
    } else {
        cerr << "sendmsg: failed to decrypt or verify mail" << endl;
    }

    vector<string> intermediates{ CREDENTIALS_PATH, INPUT_PATH, DECRYPT_PATH, VERIFY_PATH };
    // delete_files(intermediates);
	return 0;
}