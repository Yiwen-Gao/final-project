#include "conn.h"
#include "priv.h"

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
  if(clone(mail_exec, mstack+STACK, flags, &mpipe)<0){
      perror("failed to clone");
  }
  close(mpipe[0][1]);
  close(mpipe[1][0]);

  pipe(ppipe[0]);
  pipe(ppipe[1]);
  if(clone(password_exec, pstack+STACK, flags, &ppipe)<0){
      perror("failed to clone");
  }
  close(ppipe[0][1]);
  close(ppipe[1][0]);

  pipe(cpipe[0]);
  pipe(cpipe[1]);
  if(clone(ca_exec, cstack+STACK, flags, &cpipe)<0){
      perror("failed to clone");
  }
  close(cpipe[0][1]);
  close(cpipe[1][0]);
}

void getcert(string username, string password, string csr) {
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
    read(cpipe[0][0], cert, 8192);
    string c(cert, 8192);
    cout << c << endl;
  }
  else{
    end();
  }
}

void changepw(string username, string old_password, string new_password, vector<string> csr) {
  char user[50];
  char old_pass[100];
  strncpy(user, username.c_str(), 50);
  strncpy(old_pass, old_password.c_str(), 100);
  char new_pass[100];
  strncpy(new_pass, new_password.c_str(), 100);
  write(ppipe[1][1], "setp", 4);
  write(ppipe[1][1], user, 50);
  write(ppipe[1][1], old_pass, 100);
  char *result;
  read(ppipe[0][0], result, 1);
  if(!result){
    end();
  }
  write(cpipe[1][1], "make", 4);
  write(cpipe[1][1], user, 50);
  for(string line : csr){
    write(cpipe[1][1], line.c_str(), line.size());
  }
  write(cpipe[1][1], "getc", 4);
  write(cpipe[1][1], user, 50);
  char cert[8192];
  read(cpipe[0][0], cert, 8192);
  string c(cert, 8192);
  cout << c << endl;
}

void sendmsg() {
    // return certificates
}

void recvmsg() {
    // return msgs
}

int main(int argc, char **argv) {
    setup_spaces();

    const char *CA_CERT = "./certificates/ca/certs/ca.cert.pem"; 
    const char *SERVER_CERT = "./certificates/ca/intermediate/certs/localhost.cert.pem"; 
    const char *SERVER_KEY = "./certificates/ca/intermediate/private/localhost.key.pem"; 
    
    ServerConnection conn = ServerConnection(CA_CERT, SERVER_CERT, SERVER_KEY);
    string temp = conn.recv();
    REQ req = conn.parse_req(temp);
    cout << "printing details" << endl;
    cout << "user: " << req.user << endl;
    cout << "pass: " << req.password << endl;
    cout << "csr: " << endl << req.csr << endl;
    getcert(req.user, req.password, req.csr);
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
      perror("pipe closed");
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
        execl("/bin/change-pw", "change-pw", user, prev, curr, (char*)0);
      }
      else{
        waitpid(pi, &status, 0);
        if(status){
          perror("failed to change password");
        }
      }
    }
    else {
      cout << "breaking" << endl;
      break;
    }
    close(ppipe[1][0]);
    close(ppipe[0][1]);
  }
}

static int ca_exec(void *fd){
  //prepare_mntns("../../server/certificates/");
  //int **p = *((int ***)fd);
  close(cpipe[0][0]);
  close(cpipe[1][1]);
  char instr[4];
  while(true){
    if(read(cpipe[1][0], instr, 4) <= 0){
      perror("pipe closed");
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
        execl("../certificates/get-cert", "get-cert", user, (char*)0);
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
        string name = user;
        name += ".csr.pem";
        FILE *csr = fopen(name.c_str(), "wb");
        fwrite(req, length, 1, csr);
        execl("../../server/certificates/signcsr.sh", "signcsr.sh", user, (char*)0);
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
