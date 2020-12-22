#include "conn.h"
// #include <string>
// extern "C" {
//     #include <sys/wait.h>
//     #include <unistd.h>
// }

using namespace std;

#define STACK (1024*1024)
static char mstack[STACK];
static char pstack[STACK];
static char cstack[STACK];

int mpipe[2][2];
int ppipe[2][2];
int cpipe[2][2];

void setup_spaces(){
  int flags = CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWUSER | SIG_CHLD;

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
