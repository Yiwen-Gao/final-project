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
    
    // comm with server
    RecvMsgReq req = RecvMsgReq(username);
	conn.send(req.get_http_content());
    string http_content = conn.recv();
    MailResp resp = MailResp(http_content);

    // output mail
    cout << resp.sender << endl;
    for (auto it = resp.receivers.begin(); it != resp.receivers.end(); ++it) {
        cout << *it << endl;
    }
    // TODO decrypt msg!
    cout << resp.msg << endl;

	return 0;
}