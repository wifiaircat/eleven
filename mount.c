#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

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
void write_log(const char *user, const char *status);

int main(){
    //username = my name
    char *username = getlogin();
    if (username == NULL) {
        perror("getlogin: failed\n");
        //printf("[F] Failed to get username\n");
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
        strcpy(status_mesg, "[F] Failed to insmod from");
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
                perror("popen: failed");
            }

            if (fgets(cur_usr_buffer, sizeof(cur_usr_buffer), fp) != NULL) {
                char *device = strtok(cur_usr_buffer, " ");
                mount_point = strtok(NULL, " ");
                printf("[!] It's already mounted from :%s\n", mount_point);
                // addtional : show how many people in waiting queue
            }
            strcpy(status_mesg, "[F] Mount failed from");
            success = FALSE;
            fclose(fp);
        }
    }

    if (success){
        printf("[S] insmod and mount done successfully!\n");
        strcpy(status_mesg, "insmod and mount from");
    }

    printf("%s %s\n", status_mesg, username);
    write_log(username, status_mesg);

    if (success){
        pthread_t tid;
        if (pthread_create(&tid, NULL, run_check_status, NULL) != 0) {
            perror("pthread_create failed");
        } else {
            pthread_detach(tid);
        }
        printf("[*] You can now run other commands such as ./fio.sh\n");
    } else {
        printf("Do you want to wait? (y/n): ");
        char answer;
        scanf("%s", answer);
        if (strcmp(answer, "y") == 0) {
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
        perror("fork failed");
        pthread_exit(NULL);
    }
    if (pid == 0) { // child
        execl("./check_status", "check_status", NULL);
        perror("execl failed");
        exit(1);
    }
    pthread_exit(NULL);
}

void write_log(const char *user, const char *status) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) {
        perror("write_log: failed to open LOG_FILE");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] user=%s status=%s\n",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec,
        user, status);

    fclose(fp);
}