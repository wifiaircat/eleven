#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_FILE "virt_queue.txt"

void main() {
    char *username = getlogin();
    if (username == NULL) {
        perror("getlogin: failed\n");
        //printf("[F] Failed to get username\n");
    }

    FILE *fp = fopen(QUEUE_FILE, "a");
    if (fp) {
        fprintf(fp, "%s\n", username);
        fclose(fp);
    }
}