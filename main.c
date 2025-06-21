#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wait.h"
#include "check_status.h"
#include "umount.h"
#include "mount.h"

void execute_command(char* args);

int main() {
    char input[128];
    char command[64];

    while (1) {
        printf("> Enter command (mount / wait / umount / exit): ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }

        input[strcspn(input, "\n")] = 0;

        execute_command(command);
    }

    return 0;
}

void execute_command(char* args){
    if (args == NULL) return;

    if (strcmp(args, "mount") == 0) {
        mount();
        exit(0);
    } else if (strcmp(args, "wait") == 0) {
        wait();
    } else if (strcmp(args, "umount") == 0) {
        umount();
        exit(0);
    } else if (strcmp(args, "exit") == 0) {
        printf("[*] Exiting without action.\n");
        exit(0);
    } else {
        printf("[!] Unknown command.\n");
    }
    return;
}