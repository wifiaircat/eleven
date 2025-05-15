#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 
umount + rmmod + call next user + quit virt_manager
*/

int main(){
    int ret;
    
    // system() -> implement by execvp()
    // 1. umount 2. rmmod
    printf("conducting umount and rmmod...\n");

    ret = system("sudo umount /dev/nvme0n1 & sudo rmmod nvmev");
    if (ret != 0){
        printf("failed to umount and rmmod\n");
    }

    return 0;
}