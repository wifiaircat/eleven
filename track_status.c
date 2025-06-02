#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STAT_PATH "/sys/block/nvmev0n1/stat"
#define SLEEP_SEC 30
#define STAT_LINE_LEN 256

int main(){
    /*
    this function is about current user.
    check status of current user >
    if not active > ask to exit > (automatically exit(??))

    prediction : after insmod and mount (there is user rn)
    */

    // check staus point candidate 1

    // who is current user?
    FILE* fp = popen("tail -n 1 /proc/mounts", "r");
    if (fp == NULL) {
        perror("fail to popen"); return 1;
    }

    char cur_usr_buffer[512];
    fgets(cur_usr_buffer, sizeof(cur_usr_buffer), fp);

    char *p1 = strtok(cur_usr_buffer, " ");
    char *p2 = strtok(NULL, " ");
    // go to current user's pts
    
    // check status point candidate 2

    return 0;
}