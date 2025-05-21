#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define CURRENT_FILE "current_status.txt"
#define QUEUE_FILE "queue.txt"
#define SOCKET_FMT "/tmp/notify_%s.sock"
#define DEVICE "/dev/nvme0n1"
#define MOUNT_DIR "/home/aircat/nvmevirt/mnt"

// External function for logging (implemented elsewhere)
extern void write_log(const char *user, const char *status);

// Check if the resource is currently in use
int is_resource_in_use() {
    FILE *fp = fopen(CURRENT_FILE, "r");
    if (!fp) return 0;
    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "mount")) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// Update current_status.txt after successful mount
void update_status(const char *user) {
    FILE *fp = fopen(CURRENT_FILE, "w");
    if (fp) {
        fprintf(fp, "user=%s status=mount\n", user);
        fclose(fp);
    }
}

// Add user to the waiting queue
void enqueue(const char *user) {
    FILE *fp = fopen(QUEUE_FILE, "a");
    if (fp) {
        fprintf(fp, "%s\n", user);
        fclose(fp);
    }
}

// Remove and return the first user in the queue
int dequeue(char *next_user) {
    FILE *fp = fopen(QUEUE_FILE, "r");
    if (!fp) return 0;

    FILE *tmp = fopen("queue_tmp.txt", "w");
    int found = 0;
    char line[64];

    while (fgets(line, sizeof(line), fp)) {
        if (!found) {
            sscanf(line, "%s", next_user);
            found = 1;
        } else {
            fputs(line, tmp);
        }
    }

    fclose(fp);
    fclose(tmp);
    remove(QUEUE_FILE);
    rename("queue_tmp.txt", QUEUE_FILE);
    return found;
}

// Wait for notification via Unix domain socket
void wait_for_notify(const char *user) {
    char sock_path[108];
    snprintf(sock_path, sizeof(sock_path), SOCKET_FMT, user);

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    unlink(sock_path);
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    recv(sock, NULL, 1, 0); // Block until notification received

    close(sock);
    unlink(sock_path);
}

// Notify the next user in the queue
void notify_next_user() {
    char next_user[64];
    if (!dequeue(next_user)) return;

    char sock_path[108];
    snprintf(sock_path, sizeof(sock_path), SOCKET_FMT, next_user);

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (sendto(sock, "go", 2, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("notify failed");
        write_log("system", "notify failed to user");
    }

    close(sock);
}

// Mount the device to the mount point
int try_mount() {
    return system("sudo mount /dev/nvme0n1 /home/aircat/nvmevirt/mnt");
}

// Unmount the device and remove the kernel module
int release_device() {
    return system("sudo umount /dev/nvme0n1 && sudo rmmod nvmev");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <user>\n", argv[0]);
        return 1;
    }

    const char *user = argv[1];

    if (is_resource_in_use()) {
        printf("Resource is busy. Wait? (yes/no): ");
        char input[10];
        scanf("%9s", input);

        if (strcmp(input, "yes") != 0) {
            write_log(user, "ask wait → no");
            return 0;
        }

        enqueue(user);
        write_log(user, "ask wait → yes");
        wait_for_notify(user);
    }

    // Retry mount after notify
    if (!is_resource_in_use()) {
        if (try_mount() == 0) {
            update_status(user);
            write_log(user, "mount success");
            printf("Now using the resource. Press Enter to release.\n");
            getchar(); getchar(); // Wait for Enter

            if (release_device() == 0) {
                write_log(user, "umount + rmmod success");
                notify_next_user();
            } else {
                write_log(user, "umount + rmmod failed");
            }
        } else {
            write_log(user, "mount failed");
        }
    } else {
        write_log(user, "still in use after notify");
    }

    return 0;
}

