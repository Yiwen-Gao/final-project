#ifndef PRIV_H
#define PRIV_H

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <wait.h>
#include <memory.h>

#define STACK (1024*1024)


static void prepare_mntns(char *rootfs);
static void mail_exec(int *fd[2][2]);
static void password_exec(int *fd[2][2]);
static void ca_exec(int *fd[2][2]);

#endif /* PRIV_H */
