#include "conn.h"
#include "priv.h"
// #include <string>
// extern "C" {
//     #include <sys/wait.h>
//     #include <unistd.h>
// }

using namespace std;

static char mstack[STACK];
static char pstack[STACK];
static char cstack[STACK];

int mpipe[2][2];
int ppipe[2][2];
int cpipe[2][2];

void setup_spaces(){
  int flags = CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWUSER | SIGCHLD;

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



// void getcert(string username, string password, string csr_path) {
    // verify username and password
    // int status;
    // pid_t pid = fork();

    // if (pid < 0) {
    //     perror("failed to fork child process");
    // } else if (pid > 0) {
    //     waitpid(pid, &status, 0);
    //     if (status != 0) {
    //         perror("failed to verify username and/or password");
    //     }
    // } else {
    //     string path = "./passwords/verify-pw";
    //     if (execl(path.c_str(), path.c_str(), username.c_str(), password.c_str(), (char *) NULL) < 0) {
    //         perror("failed to exec verification process");
    //     }
    // }

    // generate cert from csr
    // send and store cert 
// }

void changepw() {

}

void sendmsg() {

}

void recvmsg() {

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
  unshare(CLONE_NEWNS);
  prepare_mntns("/mail/");
  int p[2][2] = *((int ***)fd);
  close(p[0][0]);
  close(p[1][1]);
  while(read(p[1][0]), buffer, sizeof(buffer)){
    // new process fork and exec to send message
  }
}

static int password_exec(void *fd){
  unshare(CLONE_NEWNS);
  prepare_mntns("/passwords/");
  int p[2][2] = *((int ***)fd);
  close(p[0][0]);
  close(p[1][1]);
  char instr[4];
  while(true){
    read(p[1][0], instr, 4);
    if(strncmp(instr, "verp", 4)){
      char user[50];
      read(p[1][0], user, 50);
      char password[100];
      read(p[1][0], password, 100);
      int status;
      pid_t p = fork();
      if(p < 0){
        perror("fork failed");
      }
      else if(p == 0){
        dup2(p[0][1], STDOUT_FILENO);
        close(p[0][1]);
        execl("/bin/verify-pw", "verify-pw", user, password, (char*)0);
      }
      else {
        waitpid(p, &status, 0);
        if(status){
          perror("failed to verify password");
        }
    }
    else if(strncmp(instr, "setp", 4)){
      char user[50];
      read(p[1][0], user, 50);
      char prev[100];
      read(p[1][0], prev, 100);
      char curr[100];
      read(p[1][0], curr, 100);
      int status;
      pid_t p = fork();
      if(p < 0){
        perror("fork failed");
      }
      else if(p == 0){
        dup2(p[0][1], STOUD_FILENO);
        close(p[0][1]);
        execl("/bin/change-pw", "change-pw", user, prev, curr, (char*)0);
      }
      else{
        waitpid(p, &status, 0);
        if(status){
          perror("failed to change password");
        }
      }
    }
    else {
      break;
    }
    close(p[1][0]);
    close(p[0][1]);
  }
}

static int ca_exec(void *fd){
  unshare(CLONE_NEWNS);
  prepare_mntns("/certificates/");
  int p[2][2] = *((int ***)fd);
  close(p[0][0]);
  close(p[1][1]);
  char instr[4];
  while(true){
    read((p[1][0]), instr, 4);
    if(strncmp(instr, "getc", 4)){
      char user[50];
      read(p[1][0], user, 50);
      int status;
      pid_t p = fork();
      if(p < 0){
        perror("fork failed");
      }
      else if(p == 0){
        dup2(p[0][1], STDOUT_FILENO);
        close(p[0][1]);
        execl("/bin/get-cert", "get-cert", user, (char*)0);
      }
      else{
        waitpid(p, &status, 0);
        if(status){
          perror("failed to retrieve certificate");
        }
      }
    }
    else if(strncmp(instr, "make", 4)){
      char user[50];
      read(p[1][0], user, 50);
      pid_t p = fork();
      if(p < 0){
        perror("fork failed");
      }
      else if(p == 0){
        int length;
        read(p[1][0], length, sizeof(int));
        char *req = malloc(length);
        read(p[1][0], req, length);
        FILE *csr = fopen(user+".csr.pem", "wb");
        fwrite(req, length, 1, csr);
        execl("/scripts/signcsr.sh", "signcsr.sh", user, (char*)0);
      }
      else{
        waitpid(p, &status, 0);
        if(status){
          perror("failed to make certificate");
        }
      }
    }
    else {
      break;
    }
  }
  close(p[1][0]);
  close(p[0][1]);
}
