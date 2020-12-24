#include "conn.h"

using namespace std;

// TODO finish implementation
CertResp::CertResp(string cert) {
    this->cert = cert;
}

string CertResp::get_body() {
    return "";
}

MailCertResp::MailCertResp(string content) {
    this->certs = str_to_vec(content);
}

string MailCertResp::get_body() {
    return "";
}

MailResp::MailResp(string content) {
    this->sender = "";
    this->receivers = vector<string>{content};
    this->msg = "";
}

string MailResp::get_body() {
    return "";
}
