//umount.c

#ifdef UMOUNT
#define UMOUNT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include "log_util.h"
#include "umount.h"

int umount();

#endif