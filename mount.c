#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "log_util.h"

#define TRUE 1
#define FALSE 0

//INFO
#define MY_PATH "/home/aircat/nvmevirt/"
#define DEVICE_NAME "/dev/nvme0n1"
#define MEMMAP_SET "memmap_start=32G memmap_size=32G cpus=7,8"

//FILE
#define QUEUE_FILE "virt_queue.txt"
#define LOG_FILE "virt_log.txt"

//CMDS
#define CMD_CUR_USER "tail -n 1 /proc/mounts"

void* run_check_status(void *arg);
void enqueue(const char *user);
int read_waitQ();

int mount(){
    //username = my name
    char *username = getlogin();
    if (username == NULL) {
        fprintf(stderr, "failed to getlogin\n");
        username = "empty";
    }

    char cur_usr_buffer[256];
    int success = TRUE;

    char status_mesg[256] = "NULL\n";
    char ins_cmd[256];
    char mnt_cmd[512];
    char *mount_point;

    // 1. insmod
    printf("[*] Conducting insmod...\n");

    snprintf(ins_cmd, sizeof(ins_cmd),
            "sudo insmod %snvmev.ko %s", MY_PATH, MEMMAP_SET);
    int ins_ret = system(ins_cmd); // success = 0
    if (ins_ret){ // insmod failed
        //printf("[F] Failed to insmod from %s\n", username);
        strcpy(status_mesg, "failed to insmod -");
        success = FALSE;
    } else { // insmod success
        // 2. mount
        printf("[*] Conducting mount...\n");

        snprintf(mnt_cmd, sizeof(mnt_cmd),
                "sudo mkfs.ext4 -F %s && sudo mount %s %s/mnt",
                DEVICE_NAME, DEVICE_NAME, MY_PATH);
        int mnt_ret = system(mnt_cmd);
        if (mnt_ret){
            // me != cur_user
            FILE* fp = popen(CMD_CUR_USER, "r"); 
            if (!fp) {
                fprintf(stderr, "failed to popen\n");
            }
            int count_waitQ = 0;

            if (fgets(cur_usr_buffer, sizeof(cur_usr_buffer), fp) != NULL) {
                char *device = strtok(cur_usr_buffer, " ");
                mount_point = strtok(NULL, " ");
                printf("[!] It's already mounted from :%s\n", mount_point);
                count_waitQ = read_waitQ();
                printf("there is %d in wait Queue\n", count_waitQ);
            }
            strcpy(status_mesg, "failed to mount -");
            success = FALSE;
            fclose(fp);
        }
    }

    if (success){
        printf("insmod and mount done successfully!\n");
        strcpy(status_mesg, "succeed to insmod and mount -");
    }

    printf("%s %s\n", status_mesg, username);
    write_log(username, status_mesg);

    if (success){
        pthread_t tid;
        int tid_ret = pthread_create(&tid, NULL, run_check_status, NULL);
        if (tid_ret != 0) {
            fprintf(stderr, "failed to pthread_create\n");
        } else {
            pthread_detach(tid);
            printf("( you can now run other commands such as ./fio_*.sh )\n");
            sleep(1);
            pthread_exit(NULL);
        }
    } else {
        printf("Do you want to wait? (yes/no): ");
        char answer[4];
        scanf("%s", answer);
        if (strcmp(answer, "yes") == 0) {
            enqueue(username);
            printf("[S] Wait completed. Exiting.\n");
        }
        exit(0);
    }
    return 0;
}

void enqueue(const char *user) {
    FILE *fp = fopen(QUEUE_FILE, "a");
    if (fp) {
        fprintf(fp, "%s\n", user);
        fclose(fp);
    }
}

void* run_check_status(void *arg) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("1st fork failed");
        pthread_exit(NULL);
    }
    if (pid == 0) { // First child
        if (setsid() < 0) {
            perror("setsid failed");
            exit(1);
        }

        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("2nd fork failed");
            exit(1);
        }
        if (pid2 == 0) { // Second child - real daemon
            char *username = getlogin();
            if (username == NULL) username = "unknown";
            write_log(username, "check_status daemon started\n");

            fclose(stdin);
            fclose(stdout);
            fclose(stderr);

            execl("./check_status", "check_status", NULL);
            
            write_log(username, "check_status execl failed\n");
            exit(1);
        }
        exit(0); // First child exit
    }
    // Parent returns immediately
    pthread_exit(NULL);
}

int read_waitQ() {
    FILE *f = fopen("virt_queue.txt", "r");
    if (f == NULL) {
        perror("open q_file failed");
        exit(1);
    }

    int count = 0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), f)) {
        count++;
    }

    fclose(f);
    return count;
}