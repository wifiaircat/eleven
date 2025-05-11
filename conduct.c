/* 
insmod + mount + excute virt_manager
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
    int ret;
    

    // system() -> implement by execvp()
    // 1. insmod
    printf("conducting insmod...\n");
    char buffer[512];

    FILE* fp = popen("tail -n 1 /proc/mounts", "r");
    if (fp == NULL) {
        perror("fail to popen"); return 1;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        char *p1 = strtok(buffer, " ");
        char *p2 = strtok(NULL, " ");
        printf("current insert module :%s\n", p2);
    }

    ret = system("sudo insmod /home/aircat/nvmevirt/nvmev.ko memmap_start=32G memmap_size=32G cpus=7,8");
    if (ret != 0){
        printf("failed to insmod\n");

    }

    // 2. mount
    printf("conducting mount...\n");
    ret = system("sudo mkfs.ext4 -F /dev/nvme0n1 && \
        sudo mount /dev/nvme0n1 /home/aircat/nvmevirt/mnt");
    if (ret != 0)
        printf("failed to mount\n");

    return 0;
}