#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* 
insmod + mount + excute virt_manager

system( ) >> implement by execvp( )
*/

int main(){
    int ret1, ret2;

    // open log file
    FILE* file = fopen("virt_log.txt", "a");
    if (file == NULL) {
        printf("Can not open log file.\nPlease retry.\n");
        exit(1);
    }

    // who
    char who_buffer[256] = "";
    FILE* cmd = popen("who", "r");
    if (cmd == NULL) { printf("can not who"); exit(1); }

    char status_mesg[256] = "";
    strcat(status_mesg, "insmod and mount from");

    // 1. insmod
    printf("conducting insmod...\n");
    char cur_usr_buffer[512];

    ret1 = system("sudo insmod /home/aircat/nvmevirt/nvmev.ko memmap_start=32G memmap_size=32G cpus=7,8");
    if (ret1 != 0){
        system("ls -l /dev/nvme*");
        printf("failed to insmod\n");
        strcat(status_mesg, "insmod failed from");
    }

    // 2. mount
    printf("conducting mount...\n");
    ret2 = system("sudo mkfs.ext4 -F /dev/nvme0n1 && sudo mount /dev/nvme0n1 /home/aircat/nvmevirt/mnt");
    if (ret2 != 0){
        FILE* fp = popen("tail -n 1 /proc/mounts", "r");
        if (fp == NULL) {
            perror("fail to popen"); return 1;
        }

        if (fgets(cur_usr_buffer, sizeof(cur_usr_buffer), fp) != NULL) {
            char *p1 = strtok(cur_usr_buffer, " ");
            char *p2 = strtok(NULL, " ");
            printf("already mounted :%s\n", p2);
            // open log file and show waiting person
        }
        printf("failed to mount\n");
        strcat(status_mesg, "mount failed from");
    }

    // failed case : broadcast to current user and ask if wait
    // for this, should give some sign

    // successful case : open log file and add log
    printf("insmod and mount done successfully!\n");
    while (fgets(who_buffer, sizeof(who_buffer), cmd) != NULL) {
        // do not work on my laptop
        fprintf(file, "%s ", status_mesg);
        fprintf(file, "%s", who_buffer);
    }
    
    // 3. manager
    printf("conducting virt_manager...\n");
    // run other file
    
    fclose(file);
    pclose(cmd);

    return 0;
}