#include "conn.h"
// #include <string>
// extern "C" {
//     #include <sys/wait.h>
//     #include <unistd.h>
// }

using namespace std;

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

    const char *CA_CERT = "../../server/certificates/ca/certs/ca.cert.pem"; // *(++argv);
    const char *SERVER_CERT = "../../server/certificates/ca/intermediate/certs/localhost.cert.pem"; // *(++argv);
    const char *SERVER_KEY = "../../server/certificates/ca/intermediate/private/localhost.key.pem"; // *(++argv);
    
    ServerConnection conn = ServerConnection(CA_CERT, SERVER_CERT, SERVER_KEY);
    cout << "au revoir" << endl;
}