#include "conn.h"

using namespace std;

const char *CA_CERT = "./trusted_certs/ca-chain.cert.pem";
const char *CLIENT_CERT = "./dummy/cert.pem";
const char *CLIENT_KEY = "./dummy/key.pem";

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
    // TODO decrypt msg!
    cout << resp.msg;

	return 0;
}