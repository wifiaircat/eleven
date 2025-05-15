#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 
umount + rmmod + call next user + quit virt_manager
*/

int main(){
    int ret;
    
    // system() -> implement by execvp()
    // 1. umount
    printf("conducting umount...\n");

    ret = system("sudo umount /dev/nvme0n1");
    if (ret != 0){
        printf("failed to umount\n");
    }

    // 2. rmmod
    printf("conducting rmmod...\n");
    ret = system("sudo rmmod nvmev");
    if (ret != 0)
        printf("failed to rmmod\n");

    return 0;
}