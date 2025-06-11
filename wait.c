#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define QUEUE_FILE "virt_queue.txt"
#define SOCKET_FMT "/tmp/notify_%s.sock"

// Wait for socket-based notification
void wait_for_notify(const char *user) {
    char sock_path[108];
    snprintf(sock_path, sizeof(sock_path), SOCKET_FMT, user);

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket failed");
        exit(1);
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    unlink(sock_path);  // Remove previous socket file if exists
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(sock);
        exit(1);
    }

    printf("[*] Waiting for notification... Will wake up when message arrives.\n");

    recv(sock, NULL, 1, 0);  // Block until notification is received

    printf("[+] Notification received! Closing socket.\n");

    close(sock);
    unlink(sock_path);
}

int main() {
    char *username = getlogin();
    if (username == NULL) {
        perror("getlogin failed");
        exit(1);
    }

    FILE *fp = fopen(QUEUE_FILE, "a");
    if (!fp) {
        perror("fopen failed");
        exit(1);
    }

    fprintf(fp, "%s\n", username);
    fclose(fp);
    printf("[*] User '%s' has been added to the queue.\n", username);

    // Wait for the notification to proceed
    wait_for_notify(username);

    return 0;
}
