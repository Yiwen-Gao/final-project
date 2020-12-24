#include "conn.h"

using namespace std;
const string CONTENT_OP = "Content-Length:";

string BaseResp::get_body() {
    return "";
}

CertResp::CertResp(string cert) {
    this->cert = cert;
}

string CertResp::get_body() {
    string data = cert + "\n";
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

MailCertResp::MailCertResp(string content) {
    this->certs = str_to_vec(content);
}

string MailCertResp::get_body() {
    string data = vec_to_str(certs);
    cout << "data:" << data;
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

MailResp::MailResp(string content) {
    vector<string> temp = str_to_vec(content);
    for (uint i = 0; i < temp.size(); i++) {
        if (i == 0) {
            sender = temp[i];
        } else if (i == 1) {
            receivers = temp[i];
        } else if (i == 2) {
            msg = temp[i];
        }
    }
}

string MailResp::get_body() {
    string data = sender + "\n" + receivers + "\n" + msg + "\n";
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}
