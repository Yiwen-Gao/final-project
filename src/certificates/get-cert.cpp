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
  if(!fread(ct, 8192, 1, cert)){
    perror("failed to read");
  }
  std::cout << ct;
}
