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
  int length = 0;
  int r = 0;
  do{
    r = fread(ct+length, 1, 8192, cert);
    length += r;
  } while(r>0);
  std::cout << length;
  std::cout << ct;
}
