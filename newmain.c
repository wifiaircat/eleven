#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char input[128];
    char command[64];

    while (1) {
        printf("> Enter command (mount / wait / umount / exit): ");
        
        if (!fgets(input, sizeof(input), stdin)) {
            printf("[!] Input error.\n");
            continue;
        }

        input[strcspn(input, "\n")] = 0;
        int count = sscanf(input, "%s", command);

        char *space = strchr(input, ' ');
        if (space && *(space + 1) != '\0') {
            printf("[!] too many arguments.\n");
            continue;
        }

        if (strcmp(command, "mount") == 0) {
            system("./mount");
            break;
        } else if (strcmp(command, "wait") == 0) {
            system("./wait");
        } else if (strcmp(command, "umount") == 0) {
            system("./umount");
            break;
        } else if (strcmp(command, "exit") == 0) {
            printf("[*] Exiting without action.\n");
            break;
        } else {
            printf("[!] Unknown command.\n");
        }
    }

    return 0;
}
