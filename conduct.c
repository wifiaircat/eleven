#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
/* 
insmod + mount + excute virt_manager

system( ) >> implement by execvp( )
*/

int main(){
    int ret1, ret2;

    char *username = getlogin();
    if (username == NULL){
        printf("[F] Failed to who\n");
    }

    // 1. insmod
    printf("[*] Conducting insmod...\n");
    char cur_usr_buffer[512];
    int success = TRUE;

    char status_mesg[256] = "NULL\n";
    ret1 = system("sudo insmod /home/aircat/nvmevirt/nvmev.ko memmap_start=32G memmap_size=32G cpus=7,8");
    if (ret1 != 0){
        system("ls -l /dev/nvme*");
        printf("[F] Failed to insmod from %s\n", username);
        success = FALSE;
    }

    // 2. mount
    printf("[*] Conducting mount...\n");
    ret2 = system("sudo mkfs.ext4 -F /dev/nvme0n1 && sudo mount /dev/nvme0n1 /home/aircat/nvmevirt/mnt");
    if (ret2 != 0){
        FILE* fp = popen("tail -n 1 /proc/mounts", "r");
        if (fp == NULL) { perror("fail to popen"); }

        if (fgets(cur_usr_buffer, sizeof(cur_usr_buffer), fp) != NULL) {
            char *p1 = strtok(cur_usr_buffer, " ");
            char *p2 = strtok(NULL, " ");
            printf("[!] It's already mounted from :%s\n", p2);
            // show waiting person
        }
        printf("[F] Mount failed from %s\n", username);
        success = FALSE;
    }

    // failed case : broadcast to current user and ask if wait
    // for this, should give some sign

    if (success){
        printf("[S] insmod and mount done successfully!\n");
        printf("insmod and mount from %s\n", username);
    }

    printf("%s %s\n", status_mesg, username);

    return 0;
}