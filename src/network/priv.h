#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <wait.h>
#include <memory.h>


static void prepare_mntns(char *rootfs);
static void mail_exec(int *fd[2][2]);
static void password_exec(int *fd[2][2]);
static void ca_exec(int *fd[2][2]);
