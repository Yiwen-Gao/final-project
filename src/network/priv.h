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
static int mail_exec(void *fd);
static int password_exec(void *fd);
static int ca_exec(void *fd);

#endif /* PRIV_H */
