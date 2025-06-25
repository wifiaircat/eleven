// log_util.c

#include <stdio.h>
#include <time.h>
#include "log_util.h"

void write_log(const char *user, const char *status) {
    FILE *fp = fopen("virt_log.txt", "a");
    if (!fp) {
        perror("write_log: failed to open virt_log.txt");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] user=%s status=%s\n",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec,
        user, status);

    fclose(fp);
}

