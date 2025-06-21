#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_FMT "/tmp/notify_%s.sock"

//FILE
#define QUEUE_FILE "virt_queue.txt"
#define LOG_FILE "virt_log.txt"

void notify_next_user(const char *cur_user);
void wait_for_notify(const char *user);
void write_log(const char *user, const char *status);

int main(){
    char *username = getlogin();
    if (username == NULL) {
        perror("getlogin: failed\n");
        //printf("[F] Failed to get username\n");
    }
    int ret;
    
    printf("[*] Conducting umount and rmmod...\n");

    ret = system("sudo umount /dev/nvme0n1 & sudo rmmod nvmev");
    if (!ret){
        notify_next_user(cur_user);
        printf("[S] umount and rmmod done successfully!\n");
    } else {
        printf("[F] Failed to umount and rmmod\n");
    }

    return 0;
}

void notify_next_user(const char *cur_user) {
    char next_user[64];
    if (!dequeue(next_user)) {
        write_log(cur_user, "no waiting user");
        return;
    }

    char sock_path[108];
    snprintf(sock_path, sizeof(sock_path), SOCKET_FMT, next_user);

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        write_log(cur_user, "socket creation failed");
        return;
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (sendto(sock, "go", 2, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("notify failed");
        write_log(cur_user, "notify failed to user");
    } else {
        write_log(cur_user, "notify success to next user");
    }

    close(sock);
}

void wait_for_notify(const char *user) {
    char sock_path[108];
    snprintf(sock_path, sizeof(sock_path), SOCKET_FMT, user);

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    unlink(sock_path);
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    recv(sock, NULL, 1, 0);  // blocking

    close(sock);
    unlink(sock_path);
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