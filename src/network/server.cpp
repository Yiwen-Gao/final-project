#include "conn.h"
#include "priv.h"
#include <vector>
#include <pwd.h>

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
  int flags = CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWUSER | CLONE_NEWNS |SIGCHLD;

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
  FILE* uid = fopen(proc.c_str(), "w");
  char *line = "0 1042 1\n";
  fwrite(line, 1, strlen(line), uid);
  fclose(uid);

  pipe(ppipe[0]);
  pipe(ppipe[1]);
  pid_t pp = clone(password_exec, pstack+STACK, flags, &ppipe);
  if(pp<0){
      perror("failed to clone");
  }
  close(ppipe[0][1]);
  close(ppipe[1][0]);
 /* proc = "/proc/";
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
  struct passwd *pw = getpwnam("cert-writer");
  uid = fopen(proc.c_str(), "w");
  string entry = "0 ";
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

void sendmsg(string user, vector<string> recips, ServerConnection conn) {
  for(string rec : recips){
    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], user.c_str(), user.size());
    char cert[8192];
    int l;
    read(cpipe[0][0], &l, sizeof(int));
    read(cpipe[0][0], cert, l);
    string c(cert, l);
    conn.send_string(c);
  }
  //string message;
  //write(cpipe[1][1], "send", 4);
  //write(cpipe[1][1], message.c_str(), message.size());
}

string recvmsg(string user) {
  write(cpipe[1][1], "recv", 4);
  write(cpipe[1][1], user.c_str(), user.size());
  int l;
  read(cpipe[0][0], &l, sizeof(int));
  if(l){
    char *message = (char*) malloc(l);
    read(cpipe[0][0], message, l);
    string m(message, l);
    return m;
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
    while (true) {
      conn.accept_client();
      cout << "LOOK AT ME IM A TWEE: " << conn.get_common_name() << endl;
      string http_content = conn.recv();
      BaseReq *req = parse_req(http_content);
      BaseResp *resp;
      if (req->type == GET_CERT) {
        GetCertReq gc_req = dynamic_cast<GetCertReq&>(*req);
        string cert = getcert(gc_req.username, gc_req.password, gc_req.csr);
        conn.send_string(cert);
        resp = CertResp(cert);
      } else if (req->type == CHANGE_PW) {
        ChangePWReq cp_req = dynamic_cast<ChangePWReq&>(*req);
        string cert = changepw(cp_req.username, cp_req.old_password, cp_req.new_password, cp_req.csr);
        resp = new CertResp(cert);
      } else if (req->type == SEND_MSG) {
        SendMsgUsersReq smu_req = dynamic_cast<SendMsgUsersReq&>(*req);
        // sendmsg(smu_req.usernames);
        resp = new MailCertResp("cert1\ncert2\ncert3");
      } else if (req->type == RECV_MSG) {
        RecvMsgReq rm_req = dynamic_cast<RecvMsgReq&>(*req);
        // string msg = recvmsg(rm_req.username);
        resp = new MailResp("addleness\nwhaledom,wamara\n\nhello!!!\n");
      } else {
        cerr << "./server: invalid http request" << endl;
      }

      //conn.send(resp->get_http_content());
      conn.close_client();
      delete req;
      delete resp;
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
  int **p = *((int ***)fd);
  close(p[0][0]);
  close(p[1][1]);
  char instr[4];
  while(read(p[1][0], instr, 4)){
    // new process fork and exec to send message
  }
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
        return 0;
        //execl("../passwords/verify-pw", "verify-pw", user, password, (char*)0);
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
      
      pid_t d =fork();
      if(d < 0){
        perror("fork failed");
      }
      else if (d == 0){
        string ag = "/CN=";
        ag += user;
        ag += "/d";
        execl("/bin/sed", "/bin/sed", "-i", ag.c_str(), "../../server/certificates/ca/intermediate/index.txt", NULL);
      }
      else {
        waitpid(d, &status, 0);
        if(status){
          perror("failed to make certificate");
        }
      }
      
      
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
