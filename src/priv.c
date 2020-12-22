#include <sched.h>

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

static void mail_exec(int **fd){
  unshare(CLONE_NEWNS);
  prepare_mntns("/mail/");
  int p[2][2] = fd;
  close(p[0][0]);
  close(p[1][1]);
  while(read(p[1][0]), buffer, sizeof(buffer)){
    // new process fork and exec to send message
  }
}

static void password_exec(int **fd){
  unshare(CLONE_NEWNS);
  prepare_mntns("/passwords/");
  int p[2][2] = fd;
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

static void ca_exec(int **fd){
  unshare(CLONE_NEWNS);
  prepare_mntns("/certificates/");
  int p[2][2] = fd;
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

#define STACK (1024*1024)
static char mstack[STACK];
static char pstack[STACK];
static char cstack[STACK];

void setup_spaces(){
  int flags = CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWUSER;
  int mpipe[2][2];
  int ppipe[2][2];
  int cpipe[2][2];

  pipe(mpipe[0]);
  pipe(mpipe[1]);
  clone(mail_exec, mstack+STACK, flags, &mpipe);
  close(mpipe[0][1]);
  close(mpipe[1][0]);

  pipe(ppipe[0]);
  pipe(ppipe[1]);
  clone(password_exec, pstack+STACK, flags, &ppipe);
  close(ppipe[0][1]);
  close(ppipe[1][0]);

  pipe(cpipe[0]);
  pipe(cpipe[1]);
  clone(ca_exec, cstack+STACK, flags, &cpipe);
  close(cpipe[0][1]);
  close(cpipe[1][0]);
}
