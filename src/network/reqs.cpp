#include "conn.h"

using namespace std;

const string CONTENT_OP = "Content-Length:";

string BaseReq::get_header() {
    return "";
}

string BaseReq::get_body() {
    return "";
}

// getcert req
GetCertReq::GetCertReq(string username, string password, string csr) {
    type = GET_CERT;
    this->username = username;
    this->password = password;
    this->csr = csr;
}

GetCertReq::GetCertReq(string content) {
    type = GET_CERT;
    vector<int> lines;
    int ind = -1;
    while((ind = content.find('\n', ind + 1)) != string::npos)
    {
        lines.push_back(ind);
    }
    if (lines.size() > 3)
    {
        this->username = content.substr(0, lines[0]);
        this->password = content.substr(lines[0] + 1, lines[1] - lines[0] - 1);
        this->csr = content.substr(lines[1] + 1);
    }
}

string GetCertReq::get_header() {
    return "GET " GET_CERT " HTTP/1.0\n";
}

string GetCertReq::get_body() {
    string data = username + "\n" + password + "\n" + csr + "\n";
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

// changepw req
ChangePWReq::ChangePWReq(string username, string old_password, string new_password, string csr) {
    type = CHANGE_PW;
    this->username = username;
    this->old_password = old_password;
    this->new_password = new_password;
    this->csr = csr;
}

ChangePWReq::ChangePWReq(string content) {
    type = CHANGE_PW;
    vector<int> lines;
    int ind = -1;
    while((ind = content.find('\n', ind + 1)) != string::npos)
    {
        lines.push_back(ind);
    }
    if (lines.size() > 3)
    {
        this->username = content.substr(0, lines[0]);
        this->old_password = content.substr(lines[0] + 1, lines[1] - lines[0] - 1);
        this->new_password = content.substr(lines[1] + 1, lines[2] - lines[1] - 1);
        this->csr = content.substr(lines[2] + 1);
    }
}

string ChangePWReq::get_header() {
    return "POST " CHANGE_PW " HTTP/1.0\n";
}

string ChangePWReq::get_body() {
    string data = username + "\n" + old_password + "\n" + new_password + "\n"; 
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

// sendmsg to retrieve users req
SendMsgReq::SendMsgReq(vector<string> usernames) {
    type = SEND_MSG;
    this->usernames = usernames;
}

SendMsgReq::SendMsgReq(string usernames) {
    type = SEND_MSG;
    this->usernames = str_to_vec(usernames);
}

string SendMsgReq::get_header() {
    return "GET " SEND_MSG " HTTP/1.0\n";
}

string SendMsgReq::get_body() {
    string data = vec_to_str(this->usernames);
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;  
}

// // sendmsg to mail req
// SendMsgMailReq::SendMsgMailReq(vector<string> msgs) {
//     type = SEND_MSG;
//     this->msgs = msgs;
// }

// SendMsgMailReq::SendMsgMailReq(string msgs) {
//     type = SEND_MSG;
//     this->msgs = str_to_vec(msgs);
// }

// string SendMsgMailReq::get_header() {
//     return "POST " SEND_MSG " HTTP/1.0\n";
// }

// string SendMsgMailReq::get_body() {
//     string data = vec_to_str(this->msgs);
//     return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;  
// }

// recvmsg req
RecvMsgReq::RecvMsgReq(string username) {
    type = RECV_MSG;
    this->username = username;
}

string RecvMsgReq::get_header() {
    return "GET " RECV_MSG " HTTP/1.0\n";
}

string RecvMsgReq::get_body() {
    string data = username + "\n";
    return CONTENT_OP + " " + to_string(data.length()) + "\n" + data;
}

// helper funcs
BaseReq *parse_req(string &http_content) {
    int i1 = http_content.find("\n");
    int i2 = http_content.substr(i1 + 1).find("\n");
    string header = http_content.substr(0, i1);
    string body = http_content.substr(i1 + 1).substr(i2 + 1);
    
    int i3 = header.find(" ");
    int i4 = header.substr(i3 + 1).find(" ");
    string verb = header.substr(0, i3);
    string type = header.substr(i3 + 1, i4);

    if (type == GET_CERT) {
        return new GetCertReq(body);
    } else if (type == CHANGE_PW) {
        return new ChangePWReq(body);
    } else if (type == SEND_MSG) {
        return new SendMsgReq(body);
    } else if (type == RECV_MSG) {
        return new RecvMsgReq(body);
    } else {
        exit(1);
    }
}
