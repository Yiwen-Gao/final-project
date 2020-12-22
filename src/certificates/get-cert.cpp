#include <unistd.h>

int main(int argc, const char* argv[]) {
  FILE* cert;
  cert = fopen(argv[1]+".cert.pem", "rb");
  if(!cert){
    perror("failed to open cert");
    exit(-1);
  }
  char ct[8192];
  if(!fread(ct, 8192, 1, cert){
    perror("failed to read");
  }
  std::cout << ct;
}
