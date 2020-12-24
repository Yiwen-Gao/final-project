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
  cout << proc << endl;
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
  cout << "about to write" << endl;
  write(ppipe[1][1], "verp", 4);
  write(ppipe[1][1], user, 50);
  write(ppipe[1][1], pass, 100);
  cout << "sent credentials" << endl;
  int result;
  read(ppipe[0][0], &result, sizeof(int));
  if(!result){
    write(cpipe[1][1], "make", 4);
    write(cpipe[1][1], user, 50);
    int l = csr.size();
    write(cpipe[1][1], &l, sizeof(int));
    write(cpipe[1][1], csr.c_str(), csr.size());
    cout << "wrote to clone" << endl;

    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], user, 50);
    cout << "getting cert" << endl;
    char cert[8192];
    read(cpipe[0][0], &l, sizeof(int));
    read(cpipe[0][0], cert, l);
    string c(cert, l);
    cout << c << endl;
    return c;
  }
  else{
    end();
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
    cout << "wrote to clone" << endl;

    write(cpipe[1][1], "getc", 4);
    write(cpipe[1][1], user, 50);
    cout << "getting cert" << endl;
    char cert[8192];
    read(cpipe[0][0], &l, sizeof(int));
    read(cpipe[0][0], cert, l);
    string c(cert, l);
    cout << c << endl;
    return c;
  }
  else{
    end();
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

void recvmsg(string user) {
  write(cpipe[1][1], "recv", 4);
  write(cpipe[1][1], user.c_str(), user.size());
  int l;
  read(cpipe[0][0], &l, sizeof(int));
  if(l){
    char *message = (char*) malloc(l);
    read(cpipe[0][0], message, l);
    string m(message, l);
    conn.send_string(m);
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
    conn.accept_client();
    string temp = conn.recv();
    REQ req = conn.parse_req(temp);
    cout << "printing details" << endl;
    cout << "user: " << req.user << endl;
    cout << "pass: " << req.password << endl;
    cout << "csr: " << endl << req.csr << endl;
    string to_send = getcert(req.user, req.password, req.csr);
    conn.send_string(to_send);
    cout << "au revoir" << endl;
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
    cout << "starting loop" << endl;
    if(read(ppipe[1][0], instr, 4)<= 0){
      cout << ppipe[1][0] << endl;
      perror("ppipe closed");
      break;
    }
    cout << "read" << endl;
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
        cout << "child starting" << endl;
        close(ppipe[0][1]);
        cout << "child happening" << endl;
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
      cout << "breaking" << endl;
      break;
    }
  }
  close(ppipe[1][0]);
  close(ppipe[0][1]);
}

static int ca_exec(void *fd){
  //prepare_mntns("../../server/certificates/");
  //int **p = *((int ***)fd);
  cout << "now in ca_exec" << endl;
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
        cout << name.c_str() << endl;
        FILE *csr = fopen(name.c_str(), "w");
        cout << fwrite(req, 1, length, csr) << endl;
        fclose(csr);

        csr = fopen(name.c_str(), "r");
        fread(req, 1, length, csr);
        cout << req << endl;
        free(req);
        fclose(csr);
        cout << getuid() << endl;
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
