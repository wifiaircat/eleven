#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void execute_command(char* args);

int main() {
    char input[128];
    char command[64];

    while (1) {
        printf("> Enter command (mount / wait / umount / exit): ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }

        char *space = strchr(input, ' ');
        if (space && *(space + 1) != '\0') {
            printf("[!] too many arguments.\n");
            continue;
        }

        input[strcspn(input, "\n")] = 0;
        int count = sscanf(input, "%s", command);

        execute_command(command);
    }

    return 0;
}

void execute_command(char* args){
    if (args == NULL) return 0;

    if (strcmp(args, "mount") == 0) {
        system("./mount");
        exit(0);
    } else if (strcmp(args, "wait") == 0) {
        system("./wait");
    } else if (strcmp(args, "umount") == 0) {
        system("./umount");
        exit(0);
    } else if (strcmp(args, "exit") == 0) {
        printf("[*] Exiting without action.\n");
        exit(0);
    } else {
        printf("[!] Unknown command.\n");
    }
    return 0;
}