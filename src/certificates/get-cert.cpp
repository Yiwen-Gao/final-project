#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

int main(int argc, const char* argv[]) {
  FILE* cert;
  std::string filename = argv[1];
  filename += ".cert.pem";
  cert = fopen(filename.c_str(), "rb");
  if(!cert){
    perror("failed to open cert");
    exit(-1);
  }
  char ct[8192];
  memset(ct, 0, 8192);
  int r = fread(ct, 1, 8192, cert);
  if(r <= 0){
    std::cerr << "failed toread err: " << errno << std::endl;
  }
  else {
    write(STDOUT_FILENO, &r, sizeof(int));
    std::cout << ct;
  }
}
