#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 
umount + rmmod + call next user + quit virt_manager
*/

int main(){
    int ret;
    
    // system() -> implement by execvp()
    // umount + rmmod
    printf("[*] Conducting umount and rmmod...\n");

    ret = system("sudo umount /dev/nvme0n1 & sudo rmmod nvmev");
    if (ret != 0){
        printf("[X] Failed to umount and rmmod\n");
    }

    return 0;
}