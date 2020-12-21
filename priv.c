#include <sched.h>

static void prepare_mntns(char *rootfs)
{
    const char *mnt = rootfs;

    if (mount(rootfs, mnt, "ext4", MS_BIND, ""))
        die("Failed to mount %s at %s: %m\n", rootfs, mnt);

    if (chdir(mnt))
        die("Failed to chdir to rootfs mounted at %s: %m\n", mnt);

    const char *put_old = ".put_old";
    if (mkdir(put_old, 0777) && errno != EEXIST)
        die("Failed to mkdir put_old %s: %m\n", put_old);

    if (syscall(SYS_pivot_root, ".", put_old))
        die("Failed to pivot_root from %s to %s: %m\n", rootfs, put_old);

    if (chdir("/"))
        die("Failed to chdir to new root: %m\n");

    if (umount2(put_old, MNT_DETACH))
        die("Failed to umount put_old %s: %m\n", put_old);
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
  prepare_mntns("/pass/");
  int p[2][2] = fd;
  close(p[0][0]);
  close(p[1][1]);
  while(read(p[1][0]), buffer, sizeof(buffer)){
    // new process fork and exec to access password store
  }
}

static void ca_exec(int **fd){
  unshare(CLONE_NEWNS);
  prepare_mntns("/ca/");
  int p[2][2] = fd;
  close(p[0][0]);
  close(p[1][1]);
  while(read(p[1][0]), buffer, sizeof(buffer)){
    // new process form and exec to access cert store
  }
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
