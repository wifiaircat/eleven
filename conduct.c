#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* 
insmod + mount + excute virt_manager

system( ) >> implement by execvp( )

whether add failed try or not???
*/

int main(){
    int ret;

    //open log file

    // 1. insmod
    printf("conducting insmod...\n");
    char buffer[512];

    ret = system("sudo insmod /home/aircat/nvmevirt/nvmev.ko memmap_start=32G memmap_size=32G cpus=7,8");
    if (ret != 0){
        system("ls -l /dev/nvme*");
        printf("failed to insmod\n");
    }

    // 2. mount
    printf("conducting mount...\n");
    ret = system("sudo mkfs.ext4 -F /dev/nvme0n1 && \
        sudo mount /dev/nvme0n1 /home/aircat/nvmevirt/mnt");
    if (ret != 0){
        FILE* fp = popen("tail -n 1 /proc/mounts", "r");
        if (fp == NULL) {
            perror("fail to popen"); return 1;
        }

        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            char *p1 = strtok(buffer, " ");
            char *p2 = strtok(NULL, " ");
            printf("already mounted :%s\n", p2);
            // open log file and show waiting person
        }
        printf("failed to mount\n");
    }

    //failed case : broadcast to current user and ask if wait

    printf("insmod and mount done successfully!\n");

    // 3. manager
    printf("conducting virt_manager...\n");
    //open log file and add log

    return 0;
}