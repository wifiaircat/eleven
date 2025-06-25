//mount.c

#ifdef MOUNT
#define MOUNT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "mount.h"

int mount();
void* run_check_status(void *arg);
void enqueue(const char *user);

#endif