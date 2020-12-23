#include "conn.h"

using namespace std;

// getcert req
GetCertReq::GetCertReq(string username, string password, string csr) {
    this->username = username;
    this->password = password;
    this->csr = csr;
}

GetCertReq::GetCertReq(string content) {
    istringstream msg(content);
    string line;
    int num = 0;

    while (getline(msg, line)) {
        if (num == 0) {
            this->username = line;
        } else if (num == 1) {
            this->password = line;
        } else if (num == 2) {
            this->csr = line;
        }
    }
}

string GetCertReq::get_header() {
    return "GET " GET_CERT " HTTP/1.0\n";
}

string GetCertReq::get_body() {
    return this->username + "\n" + this->password + "\n" + this->csr + "\n";
}

// changepw req
ChangePWReq::ChangePWReq(string username, string old_password, string new_password, string csr) {
    this->username = username;
    this->old_password = old_password;
    this->new_password = new_password;
    this->csr = csr;
}

ChangePWReq::ChangePWReq(string content) {
    istringstream msg(content);
    string line;
    int num = 0;

    while (getline(msg, line)) {
        if (num == 0) {
            this->username = line;
        } else if (num == 1) {
            this->old_password = line;
        } else if (num == 2) {
            this->new_password = line;
        } else if (num == 3) {
            this->csr = line;
        }
    }
}

string ChangePWReq::get_header() {
    return "POST " CHANGE_PW " HTTP/1.0\n";
}

string ChangePWReq::get_body() {
    return this->username + "\n" + this->old_password + "\n" + this->new_password + "\n";
}

// sendmsg to retrieve users req
SendMsgUsersReq::SendMsgUsersReq(vector<string> usernames) {
    this->usernames = usernames;
}

SendMsgUsersReq::SendMsgUsersReq(string usernames) {
    this->usernames = str_to_vec(usernames);
}

string SendMsgUsersReq::get_header() {
    return "GET " SEND_MSG " HTTP/1.0\n";
}

string SendMsgUsersReq::get_body() {
    return vec_to_str(this->usernames);
}

// sendmsg to mail req
SendMsgMailReq::SendMsgMailReq(string msgs) {
    this->msgs = str_to_vec(msgs);
}

void SendMsgMailReq::add_msg(string msg) {
    this->msgs.push_back(msg);
}

string SendMsgMailReq::get_header() {
    return "POST " SEND_MSG " HTTP/1.0\n";
}

string SendMsgMailReq::get_body() {
    return vec_to_str(this->msgs);  
}

// recvmsg req
RecvMsgReq::RecvMsgReq(string username) {
    this->username = username;
}

string RecvMsgReq::get_header() {
    return "GET " RECV_MSG " HTTP/1.0\n";
}

string RecvMsgReq::get_body() {
    return this->username + "\n";
}

// helper funcs
BaseReq parse_req(string &http_content) {
    int i = http_content.find("\n");
    string header = http_content.substr(0, i);
    string body = http_content.substr(i + 1);
    
    int j = header.find(" ");
    int k = header.substr(j + 1).find(" ");
    string verb = header.substr(0, j);
    string type = header.substr(j + 1, k);

    if (type == GET_CERT) {
        return GetCertReq(body);
    } else if (type == CHANGE_PW) {
        return ChangePWReq(body);
    } else if (type == SEND_MSG) {
        if (verb == "GET") {
            return SendMsgUsersReq(body);
        } else {
            return SendMsgMailReq(body);
        }
    } else if (type == RECV_MSG) {
        return RecvMsgReq(body);
    } else {
        exit(1);
    }
}
