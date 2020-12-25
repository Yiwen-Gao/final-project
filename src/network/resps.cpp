#include "conn.h"

using namespace std;

const string CONTENT_OP = "Content-Length:";

string BaseResp::get_header() {
    return "HTTP/1.0 200 OK";
}

string BaseResp::get_body() {
    return "";
}

// recv own cert resp
CertResp::CertResp(string cert) {
    this->cert = cert;
}

string CertResp::get_body() {
    string data = cert + "\n";
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

// recv recipient certs resp
MailCertResp::MailCertResp(vector<string> certs) {
    this->certs = certs;
}

MailCertResp::MailCertResp(string content) {
    vector<string> lines = str_to_vec(content);
    string curr = "";
    for (string line : lines)
    {
        curr += line + "\n";
        if (line.find("END CERTIFICATE") != string::npos)
        {
            this->certs.push_back(curr);
            curr = "";
        }
    }
}

string MailCertResp::get_body() {
    string data = vec_to_str(certs);
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

// recvmsg resp
MailResp::MailResp(string content) {
    int i = content.find("\n\n");
    address = content.substr(0, i);
    msg = content.substr(i + 2);
}

string MailResp::get_body() {
    string data = address + "\n\n" + msg + "\n";
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

string remove_headers(string &http_content) {
    int i1 = http_content.find("\n");
    int i2 = http_content.substr(i1 + 1).find("\n");
    return http_content.substr(i1 + 1).substr(i2 + 1);
}
