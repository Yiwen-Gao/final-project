#include "conn.h"
#include "priv.h"
#include <vector>
#include <pwd.h>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>
#include "../mail/mail_utils.h"
namespace fs = std::filesystem;

#define MAILBOX_NAME_MAX 255
#define MAIL_FROM_MAX 12
#define RCPT_TO_MAX 10

const std::string mail_prefix = "../../server/mail/";


using namespace std;

static char mstack[STACK];
static char pstack[STACK];
static char cstack[STACK];

int mpipe[2][2];
int ppipe[2][2];
int cpipe[2][2];

void end(){
  close(mpipe[0][0]);
  close(mpipe[1][1]);
  close(ppipe[0][0]);
  close(ppipe[1][1]);
  close(cpipe[0][0]);
  close(cpipe[1][1]);
  exit(0);
}
void setup_spaces(){
  //int flags = CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWUSER | CLONE_NEWNS |SIGCHLD;
  int flags = CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWNS |SIGCHLD;

  pipe(mpipe[0]);
  pipe(mpipe[1]);
  int mp = clone(mail_exec, mstack+STACK, flags, &mpipe);
  if(mp < 0){
      perror("failed to clone");
  }
  close(mpipe[0][1]);
  close(mpipe[1][0]);
  string proc = "/proc/";
  proc += to_string(mp);
  proc += "/uid_map";
  struct passwd *pw = getpwnam("mail-writer");
  FILE *uid = fopen(proc.c_str(), "w");
  string entry = "0 ";
  entry += to_string((int)pw->pw_uid);
  entry += " 1\n";
  fwrite(entry.c_str(), 1, strlen(entry.c_str()), uid);
  fclose(uid);

  pipe(ppipe[0]);
  pipe(ppipe[1]);
  pid_t pp = clone(password_exec, pstack+STACK, flags, &ppipe);
  if(pp<0){
      perror("failed to clone");
  }
  close(ppipe[0][1]);
  close(ppipe[1][0]);
  /*proc = "/proc/";
  proc += pp;
  proc += "/uid_map";
  uid = fopen(proc.c_str(), "w");
  line = "0 1042 1\n";
  fwrite(line, 1, strlen(line), uid);
  fclose(uid);*/

  pipe(cpipe[0]);
  pipe(cpipe[1]);
  pid_t cp = clone(ca_exec, cstack+STACK, flags, &cpipe);
  if(cp<0){
      perror("failed to clone");
  }
  close(cpipe[0][1]);
  close(cpipe[1][0]);
  proc = "/proc/";
  proc += to_string(cp);
  proc += "/uid_map";
  pw = getpwnam("cert-writer");
  uid = fopen(proc.c_str(), "w");
  entry = "0 ";
  entry += to_string((int)pw->pw_uid);
  entry += " 1\n";
  fwrite(entry.c_str(), 1, strlen(entry.c_str()), uid);
  fclose(uid);
}



string getcert(string username, string password, string csr) {
  char user[50];
  char pass[100];
  strncpy(user, username.c_str(), 50);
  strncpy(pass, password.c_str(), 100);
  write(ppipe[1][1], "verp", 4);
  write(ppipe[1][1], user, 50);
  write(ppipe[1][1], pass, 100);
  int result;
  read(ppipe[0][0], &result, sizeof(int));
  if(!result){
    write(cpipe[1][1], "make", 4);
    write(cpipe[1][1], user, 50);
    int l = csr.size();
    write(cpipe[1][1], &l, sizeof(int));
    write(cpipe[1][1], csr.c_str(), csr.size());

    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], user, 50);
    char cert[8192];
    read(cpipe[0][0], &l, sizeof(int));
    read(cpipe[0][0], cert, l);
    string c(cert, l);
    return c;
  }
  else{
    //end();
    return "";
  }
}

string changepw(string username, string old_password, string new_password, string csr) {
  char user[50];
  char old_pass[100];
  strncpy(user, username.c_str(), 50);
  strncpy(old_pass, old_password.c_str(), 100);
  char new_pass[100];
  strncpy(new_pass, new_password.c_str(), 100);
  write(ppipe[1][1], "setp", 4);
  write(ppipe[1][1], user, 50);
  write(ppipe[1][1], old_pass, 100);
  int result;
  read(ppipe[0][0], &result, sizeof(int));
  if(!result){
    write(cpipe[1][1], "make", 4);
    write(cpipe[1][1], user, 50);
    int l = csr.size();
    write(cpipe[1][1], &l, sizeof(int));
    write(cpipe[1][1], csr.c_str(), csr.size());

    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], user, 50);
    char cert[8192];
    read(cpipe[0][0], &l, sizeof(int));
    read(cpipe[0][0], cert, l);
    string c(cert, l);
    return c;
  }
  else{
    //end();
    return "";
  }
}

void sendmsg_test(string user, vector<string> recips, vector<char *> messages) {
  string header = user + "\n";
  for(string rec : recips){
    header += rec + ",";
    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], user.c_str(), 50);
    char cert[8192];
    int l;
    read(cpipe[0][0], &l, sizeof(int));
    read(cpipe[0][0], cert, l);
    string c(cert, l);
  }
  header += "\n";
  vector<int> sizes;
  int index = 0;
  for(string rec : recips){
    sizes.push_back( strlen(messages[index]) );
    write(mpipe[1][1], "send", 4);
    write(mpipe[1][1], rec.c_str(), 50);
    int curr_len = header.size() + sizes[index];
    write(mpipe[1][1], &curr_len, sizeof(int));
    write(mpipe[1][1], header.c_str(), header.size());
    write(mpipe[1][1], messages[index], sizes[index]);
  }
}

int sendmsg(string user, vector<string> recips, ServerConnection conn) {
  if (user == "dummy")
  {
    return -1;
  }
  cout << "calling sendmsg" << endl;
  cout << "this is called on user: " << user << endl << "recip: " << recips[0] << endl;
  conn.send("HTTP/1.0 200 OK\nContent-Length: idgaf\n");
  string header = user + "\n";
  for(string rec : recips){
    header += rec + ",";
    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], user.c_str(), 50);
    char cert[8192];
    int l;
    read(cpipe[0][0], &l, sizeof(int));
    read(cpipe[0][0], cert, l);
    string c(cert, l);
    conn.send_string(c);
  }
  conn.send_string("\n");
  header += "\n";
  vector<int> sizes;
  vector<char *> messages = conn.get_sendmsg_messages(recips.size(), sizes);
  int index = 0;
  for(string rec : recips){
    write(mpipe[1][1], "send", 4);
    write(mpipe[1][1], rec.c_str(), 50);
    int curr_len = header.size() + sizes[index];
    write(mpipe[1][1], &curr_len, sizeof(int));
    write(mpipe[1][1], header.c_str(), header.size());
    write(mpipe[1][1], messages[index], sizes[index]);
    free(messages[index++]);
  }
  return 0;
}

string recvmsg_test(string user, string &cert_in) {
  write(mpipe[1][1], "recv", 4);
  write(mpipe[1][1], user.c_str(), user.size());
  int l;
  read(mpipe[0][0], &l, sizeof(int));
  if(l){
    char *message = (char*) malloc(l);
    read(mpipe[0][0], message, l);
    string m(message, l);
    int new_line = m.find('\n');
    string sender = m.substr(0, new_line);
    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], sender.c_str(), 50);
    char cert[8192];
    int len_cert;
    read(cpipe[0][0], &len_cert, sizeof(int));
    read(cpipe[0][0], cert, len_cert);
    string c(cert, len_cert);
    cert_in = c;
    return m;
  }
}

void recvmsg(string user, ServerConnection conn) {
  write(mpipe[1][1], "recv", 4);
  write(mpipe[1][1], user.c_str(), user.size());
  int l;
  read(mpipe[0][0], &l, sizeof(int));
  if(l){
    char *message = (char*) malloc(l);
    read(mpipe[0][0], message, l);
    string m(message, l);
    int new_line = m.find('\n');
    string sender = m.substr(0, new_line);
    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], sender.c_str(), 50);
    char cert[8192];
    int len_cert;
    read(cpipe[0][0], &len_cert, sizeof(int));
    read(cpipe[0][0], cert, len_cert);
    string c(cert, len_cert);
    conn.send_bytes((char *)&len_cert, sizeof(int));
    conn.send_string(c);
    conn.send_bytes((char *)&l, sizeof(int));
    conn.send_bytes(message, l);
  }
}

int main(int argc, char **argv) {
    // if (argc < 4) {
    //     perror("usage: ./server <ca_cert> <server_cert> <server_key>");
    //     exit(1);
    // }
    setup_spaces();

    const char *CA_CERT = "../../server/certificates/ca/certs/ca.cert.pem"; // *(++argv);
    const char *SERVER_CERT = "../../server/certificates/ca/intermediate/certs/localhost.cert.pem"; // *(++argv);
    const char *SERVER_KEY = "../../server/certificates/ca/intermediate/private/localhost.key.pem"; // *(++argv);
    
    ServerConnection conn = ServerConnection(CA_CERT, SERVER_CERT, SERVER_KEY);
    pid_t p = fork();
    if(p < 0){
      return -1;
    }
    else if(p){
      int status;
      waitpid(p, &status, 0);
      return 0;
    }
    unshare(CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID);
    /*string user = "muermo";
    string recv_user = "muermo";
    char *message = "hello there, I am muermo\n\n";
    vector<string> recipients;
    recipients.push_back(recv_user);
    vector<char *> messages;
    messages.push_back(message);
    sendmsg_test(user, recipients, messages);
    string cert;
    string received = recvmsg_test(recv_user, cert);
    cout << received << endl << endl;
    cout << cert << endl;*/
    while (true) {
      cout << "is this happening? it shouldn't be" << endl;
      conn.accept_client();
      string http_content = conn.recv();
      BaseReq *req = parse_req(http_content);
      BaseResp *resp;
      if (req->type == GET_CERT) {
        GetCertReq gc_req = dynamic_cast<GetCertReq&>(*req);
        string cert = getcert(gc_req.username, gc_req.password, gc_req.csr);
        conn.send_string(cert);
        //resp = new CertResp(cert);
      } else if (req->type == CHANGE_PW) {
        ChangePWReq cp_req = dynamic_cast<ChangePWReq&>(*req);
        string cert = changepw(cp_req.username, cp_req.old_password, cp_req.new_password, cp_req.csr);
        conn.send_string(cert);
        //resp = new CertResp(cert);
      } else if (req->type == SEND_MSG) {
        SendMsgReq smu_req = dynamic_cast<SendMsgReq&>(*req);
        sendmsg(conn.get_common_name(), smu_req.usernames, conn);
        //resp = new MailCertResp("cert1\ncert2\ncert3");
        // TODO remove
        //conn.send(resp->get_http_content());
        //string msg = conn.recv();
        //conn.send("OK");
      } else if (req->type == RECV_MSG) {
        RecvMsgReq rm_req = dynamic_cast<RecvMsgReq&>(*req);
        cout << "username: " << rm_req.username << endl;
        recvmsg(rm_req.username, conn);
        //resp = new MailResp("addleness\nwhaledom,wamara\n\nhello!!!\n");
      } else {
        cerr << "./server: invalid http request" << endl;
      }

      // conn.send(resp->get_http_content());
      conn.close_client();
      delete req;
      //delete resp;
    }
}


static void prepare_mntns(char *rootfs)
{
    const char *mnt = rootfs;

    if (mount(rootfs, mnt, "ext4", MS_BIND, ""))
        exit(-1);

    if (chdir(mnt))
        exit(-1);

    const char *put_old = ".put_old";
    if (mkdir(put_old, 0777) && errno != EEXIST)
        exit(-1);

    if (syscall(SYS_pivot_root, ".", put_old))
        exit(-1);

    if (chdir("/"))
        exit(-1);

    if (umount2(put_old, MNT_DETACH))
        exit(-1);
}

static int mail_exec(void *fd){
  //prepare_mntns("/mail/");
  //int **p = *((int ***)fd);
  close(mpipe[0][0]);
  close(mpipe[1][1]);
  char instr[4];
  while(true){
    if(read(mpipe[1][0], instr, 4)<= 0){
      //perror("ppipe closed");
      break;
    }
    if(!strncmp(instr, "send", 4)){
      char user[50];
      read(mpipe[1][0], user, 50);
      int l;
      read(mpipe[1][0], &l, sizeof(int));
      char *message = (char*)malloc(l);
      read(mpipe[1][0], message, l);
        string mail_box = user;
        if (!validMailboxChars(mail_box) || mail_box.length() > MAILBOX_NAME_MAX || !doesMailboxExist(mail_box)){
          return 1;
        }
        string next_file_num = getNextNumber(mail_box);
        string mail_path = newMailPath(mail_box, next_file_num);
        FILE *mes = fopen(mail_path.c_str(), "w");
        if(mes){
          fwrite(message, 1, l, mes);
        }
        free(message);
        fclose(mes);
    }
    else if(!strncmp(instr, "recv", 4)){
      char user[50];
      read(mpipe[1][0], user, 50);
      pid_t pi = fork();
      if(pi < 0){
        perror("fork failed");
      }
      else if(pi==0){
        cout << "out:" << user << ":" << endl;
        dup2(mpipe[0][1], STDOUT_FILENO);
        close(mpipe[0][1]);
        close(mpipe[1][0]);
        execl("../mail/get-msg", "get-msg", user, (char*)0);
      }
      else {
        int status;
        waitpid(pi, &status, 0);
        if(status){
          perror("failed to get mail");
        }
      }
    }
    else {
      break;
    }
  }
  close(mpipe[0][1]);
  close(mpipe[1][0]);
}

static int password_exec(void *fd){
  //prepare_mntns("../../server/passwords/");
  //int **p = *((int ***)fd);
  close(ppipe[0][0]);
  close(ppipe[1][1]);
  char instr[4];
  while(true){
    if(read(ppipe[1][0], instr, 4)<= 0){
      perror("ppipe closed");
      break;
    }
    if(!strncmp(instr, "verp", 4)){
      char user[50];
      read(ppipe[1][0], user, 50);
      char password[100];
      read(ppipe[1][0], password, 100);
      int status;
      pid_t pi = fork();
      if(pi < 0){
        perror("fork failed");
      }
      else if(pi == 0){
        //dup2(ppipe[0][1], STDOUT_FILENO);
        close(ppipe[0][1]);
        //return 0;
        execl("../passwords/verify-pw", "verify-pw", user, password, (char*)0);
        cout << errno << endl;
      }
      else {
        waitpid(pi, &status, 0);
        if(status){
          cout << "failed to verify password" << endl;
        }
        write(ppipe[0][1], &status, sizeof(int));
      }
    }
    else if(!strncmp(instr, "setp", 4)){
      char user[50];
      read(ppipe[1][0], user, 50);
      char prev[100];
      read(ppipe[1][0], prev, 100);
      char curr[100];
      read(ppipe[1][0], curr, 100);
      int status;
      pid_t pi = fork();
      if(pi < 0){
        perror("fork failed");
      }
      else if(pi == 0){
        dup2(ppipe[0][1], STDOUT_FILENO);
        close(ppipe[0][1]);
        execl("../passwords/change-pw", "change-pw", user, prev, curr, (char*)0);
      }
      else{
        waitpid(pi, &status, 0);
        if(status){
          perror("failed to change password");
        }
        write(ppipe[0][1], &status, sizeof(int));
      }
    }
    else {
      break;
    }
  }
  close(ppipe[1][0]);
  close(ppipe[0][1]);
}

static int ca_exec(void *fd){
  //prepare_mntns("../../server/certificates/");
  //int **p = *((int ***)fd);
  close(cpipe[0][0]);
  close(cpipe[1][1]);
  setuid(0);
  char instr[4];
  while(true){
    if(read(cpipe[1][0], instr, 4) <= 0){
      perror("cpipe closed");
      break;
    }
    if(!strncmp(instr, "getc", 4)){
      char user[50];
      read(cpipe[1][0], user, 50);
      int status;
      pid_t pi = fork();
      if(pi < 0){
        perror("fork failed");
      }
      else if(pi == 0){
        dup2(cpipe[0][1], STDOUT_FILENO);
        close(cpipe[0][1]);
        string location = "../../server/certificates/ca/intermediate/certs/";
        location += user;
        execl("../certificates/get-cert", "get-cert", location.c_str(), (char*)0);
      }
      else{
        waitpid(pi, &status, 0);
        if(status){
          perror("failed to retrieve certificate");
        }
      }
    }
    else if(!strncmp(instr, "make", 4)){
      char user[50];
      read(cpipe[1][0], user, 50);
      int status;
      
      string index_txt = "", line;
      ifstream ifs ("../../server/certificates/ca/intermediate/index.txt", ifstream::in);
      string to_find = "CN=";
      to_find += user;
      while (getline(ifs, line))
      {
            if (line.empty())
            {
                index_txt += "\n";
            }
            else if(line.find(to_find) == string::npos)
            {
                index_txt += line + "\n";
            }
      }
      ifs.close();
      ofstream ofs ("../../server/certificates/ca/intermediate/index.txt", ofstream::out);
      ofs << index_txt;
      ofs.close();
      
      pid_t pi = fork();
      if(pi < 0){
        perror("fork failed");
      }
      else if(pi == 0){
        int length;
        read(cpipe[1][0], &length, sizeof(int));
        char *req = (char *)malloc(length);
        read(cpipe[1][0], req, length);
        string location = "../../server/certificates/ca/intermediate/";
        string name = location + "csr/" + user;
        name += ".csr.pem";
        FILE *csr = fopen(name.c_str(), "w");
        fwrite(req, 1, length, csr);
        fclose(csr);

        csr = fopen(name.c_str(), "r");
        fread(req, 1, length, csr);
        free(req);
        fclose(csr);
        execl("../../server/certificates/signcsr.sh", "signcsr.sh", location.c_str(), user, (char*)0);
      }
      else{
        waitpid(pi, &status, 0);
        if(status){
          perror("failed to make certificate");
        }
      }
    }
    else {
      break;
    }
  }
  close(cpipe[1][0]);
  close(cpipe[0][1]);
}

/*
Input: (std::string) A string.
Output: (boolean) Whether the string's characters are all alphabetic.
Gives back whether a string only has alphabetic characters.
*/
bool isAlpha(const std::string &str)
{
    bool alpha = true;

    // Make sure string has a single char
    if (str.empty())
    {
        alpha = false;
    }

    // Check incrementally that all characters are alphabetical
    for(char const &c : str)
    {
        if( !alpha || !std::isalpha(c) )
        {
            alpha = false;
            break;
        }        
    }

    return alpha;
}

/*
Input: (std::string) A string.
Output: (boolean) Whether a string's characters are all valid mailbox chars.
Checks whether characters are included in upper and lower case letters, digits, +, -, and _
*/
bool validMailboxChars(const std::string &str)
{    
    if (str.empty())
    {
        return false;
    }

    // First character must be alphabetic
    if (!std::isalpha(str[0]))
    {
        return false;
    }

    for(char const &c : str)
    {
        if (!std::isalpha(c) && 
        !std::isdigit(c) && 
        c != '+' && c != '-' && c != '_')
        {
            return false;
        }
    }

    return true;
}

/*
Input: (std::string) Input line from mail-in parsed file.
Output: (bool) Whether the line is in valid MAIL FROM format.
Returns an indication of whether line is in correct MAIL FROM format.
*/
bool checkMailFrom(const std::string &line)
{
    // Regex for MAIL FROM:<username>
    std::regex mail_from_regex("^[mM][aA][iI][lL] [fF][rR][oO][mM]:<.{1,255}>$");

    // Check characters do not exceed maximum possible length
    if (line.length() > MAILBOX_NAME_MAX + MAIL_FROM_MAX)
    {
        return false;
    }

    // Check MAIL FROM:<username> format
    if ( !std::regex_match(line, mail_from_regex) )
    {
        return false;
    }

    return true;
}

/*
Input: (std::string) Input line from mail-in parsed file.
Output: (bool) Whether the line is in valid DATA (delimiter) format.
Returns an indication of whether line is in correct DATA format.
*/
bool checkDataDelimiter(const std::string &line)
{
    // Regex for MAIL FROM:<username>
    std::regex rgx("^[dD][aA][tT][aA]$");

    // Check characters do not exceed maximum possible length
    if (line.length() > MAILBOX_NAME_MAX + RCPT_TO_MAX)
    {
        return false;
    }

    // Check DATA format
    if ( !std::regex_match(line, rgx) )
    {
        return false;
    }

    return true;
}

/*
Input: (std::string) Input line from mail-in parsed file.
Output: (bool) Whether the line is in valid RCPT TO format.
Returns an indication of whether line is in correct RCPT TO format.
*/
bool checkRcptTo(const std::string &line)
{
    // Regex for MAIL FROM:<username>
    std::regex rgx("^[rR][cC][pP][tT] [tT][oO]:<.{1,255}>$");

    // Check characters do not exceed maximum possible length
    if (line.length() > MAILBOX_NAME_MAX + MAIL_FROM_MAX)
    {
        return false;
    }

    // Check RCP TO:<username> format
    if ( !std::regex_match(line, rgx) )
    {
        return false;
    }

    return true;
}

/*
Input: (std::string) Input line from mail-in parsed file.
Output: (bool) Whether this line is the end of message indicator.
Returns boolean of whether line is the end of message indicator.
*/
bool checkEndOfMessage(const std::string &line)
{
    // Regex for MAIL FROM:<username>
    if(line == ".")
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
Input: (std::string) Control line from mail message.
Output: (std::string) Parsed username from middle of brackets.
Returns the username in the brackets.
std::string extractUsername(const std::string &line)
{
    // Regex for brackets
    std::regex rgx("<(.*?)>");
    std::smatch match;
    std::string username;
    std::regex_search(line, match, rgx);
    std::ssub_match sub_match = match[1];
    username = sub_match.str();
    return username;
}
*/

/*
Input: (std::string) Control line from mail message.
Output: (std::string) Parsed username from middle of brackets.
Returns the username in the brackets.
*/
std::string extractUsername(const std::string &line)
{
    bool readUsername = false;
    std::string username;

    for(char const &c : line)
    {
        if ( readUsername )
        {
            username += c;
        }
        else if ( c == '<')
        {
            readUsername = true;
        }
    }

    username = username.substr(0, username.size() - 1);
    return username;
}

/* 
Input: (std::string) Mailbox name.
Output: (bool) Whether mailbox directory exists in system or not.
Checks if mailbox path exists (used by mail-out)
*/
bool doesMailboxExist(const std::string &s)
{
    // Must check valid mailbox characters first
    if ( !validMailboxChars(s) )
    {
        return false;
    }

    std::string mailbox_path = mail_prefix + s;
    struct stat buffer;
    return (stat (mailbox_path.c_str(), &buffer) == 0);
}

/*
Input: (std::string) Mailbox name, (std::string) File name.
Output: (std::string) Path to new writing new file.
Gives back the appropriate path to write the new mailed file to.
*/
std::string newMailPath(const std::string &mailbox_name, const std::string &file_name)
{
    std::string mailbox_path = mail_prefix + mailbox_name + "/" + file_name;
    return mailbox_path;
}

/*
Input: (std::string) A string.
Output: (boolean) Whether the string's characters are all numeric.
Gives back whether a string only has numeric characters.
*/
bool isNumeric(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

/* 
Input:  (std::string) Mailbox name.
Output: (std::string) Next message name in current mailbox.
Checks the current highest numbering of the messages in the mailbox
and returns the next number.
*/

std::string get_stem(const std::filesystem::path &p) { return (p.stem().string()); }
std::string getCurrNumber(const std::string &mailbox_name)
{
    std::string mailbox_path = mail_prefix + mailbox_name;
    std::vector<std::string> files;

    // Iterate over the directory
    for(const auto & entry : std::filesystem::directory_iterator(mailbox_path))
    {
        try
        {
            files.push_back(get_stem(entry.path()));
        }
        catch(...)
        {
            return "ERROR";
        }
    }

    // Get the maximum number file
    int max = 0;
    for(std::string file_name : files)
    {
        // Check that file is appropriate length
        if (file_name.length() > 5)
        {
            return "ERROR";
        }

        // Check that file ONLY has numbers
        if (!isNumeric(file_name))
        {
            return "ERROR";
        }
        
        file_name.erase(0, file_name.find_first_not_of('0'));
        int num;

        // Check that file can be converted to a number
        try
        {
            num = std::stoi(file_name);
        }
        catch(std::invalid_argument &e)
        {
            return "ERROR";
        }
        
        if (num > max)
        {
            max = num;
        }
    }

    // Format new file number in ##### format
    int new_num = max;
    std::string num_str = std::to_string(new_num);
    while(num_str.length() < 5)
    {
        num_str = "0" + num_str;
    }

    return num_str;
}

std::string getNextNumber(const std::string &mailbox_name)
{
    std::string mailbox_path = mail_prefix + mailbox_name;
    std::vector<std::string> files;

    // Iterate over the directory
    for(const auto & entry : std::filesystem::directory_iterator(mailbox_path))
    {
        try
        {
            files.push_back(get_stem(entry.path()));
        }
        catch(...)
        {
            return "ERROR1";
        }
    }

    // Get the maximum number file
    int max = 0;
    for(std::string file_name : files)
    {
        // Check that file is appropriate length
        if (file_name.length() > 5)
        {
            continue;
        }

        // Check that file ONLY has numbers
        if (!isNumeric(file_name))
        {
            continue;
        }
        
        file_name.erase(0, file_name.find_first_not_of('0'));
        int num;

        // Check that file can be converted to a number
        try
        {
            num = std::stoi(file_name);
        }
        catch(std::invalid_argument &e)
        {
            return "ERROR4";
        }
        
        if (num > max)
        {
            max = num;
        }
    }

    // Format new file number in ##### format
    int new_num = max + 1;
    std::string num_str = std::to_string(new_num);
    while(num_str.length() < 5)
    {
        num_str = "0" + num_str;
    }

    return num_str;
}

std::vector<std::string> ipcHelper(FullMessage fullMessage)
{
    std::vector<std::string> writeList;

    // FROM line
    std::string headerFrom;
    headerFrom = "From: " + fullMessage.mailFrom + "\n";
    writeList.push_back(headerFrom);

    // TO line
    std::string headerTo = "To: ";
    for (std::string rcpt : fullMessage.rcptTo)
    {
        headerTo = headerTo + rcpt + ", ";
    }
    headerTo.pop_back();
    headerTo.pop_back();
    headerTo = headerTo + "\n";
    writeList.push_back(headerTo);

    // Line break character
    writeList.push_back("\n");

    // Message lines
    for(std::string msgLine : fullMessage.data)
    {
        if ( msgLine == "\n")
        {
            writeList.push_back("\n");
        } 
        else
        {
            std::string formattedMsg = msgLine + "\n";
            writeList.push_back(formattedMsg);
        }
    }

    return writeList;
}
