#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char command[64];

    while (1) {
        printf("Enter command (mount / wait / umount / exit): ");
        scanf("%s", command);

        if (strcmp(command, "mount") == 0) {
            system("./mount");
        } else if (strcmp(command, "wait") == 0){
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
