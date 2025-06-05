#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CMD_CUR_USER "tail -n 1 /proc/mounts"
#define CMD_DMESG "dmesg | grep nvmevirt | tail -n 1"
#define LOGLEVEL_PATH "/proc/sys/kernel/printk"
#define UPTIME_PATH "/proc/uptime"
#define THRESHOLD_SEC 30

int main() {
    /*
    this function is about current user.
    check status of current user >
    if not active > ask to exit > (automatically exit(??))

    prediction : after insmod and mount (there is user rn)
    */
    // 1. who is current user
    FILE *fp = popen(CMD_CUR_USER, "r");
    if (!fp) {
        perror("fail to popen"); return 1;
    }

    char cur_usr_buffer[512];
    if (!fgets(cur_usr_buffer, sizeof(cur_usr_buffer), fp)) {
        fprintf(stderr, "[X] Failed to read /proc/mounts\n");
        pclose(fp);
        return 1;
    }
    pclose(fp);

    char *device = strtok(cur_usr_buffer, " ");
    char *mount_point = strtok(NULL, " ");

    if (!device || !mount_point) {
        fprintf(stderr, "[X] Failed to parse mount entry.\n");
        return 1;
    }

    printf("[+] Detected device: %s mounted on %s\n", device, mount_point);

    if (strstr(device, "nvmev0n1") == NULL) {
        printf("[-] Not nvmevirt device. Exit.\n");
        return 0;
    }

    // 2. check kernel log lv
    int console_loglevel = -1;
    fp = fopen(LOGLEVEL_PATH, "r");
    if (!fp || fscanf(fp, "%d", &console_loglevel) != 1) {
        fprintf(stderr, "[X] Failed to read loglevel\n");
        if (fp) fclose(fp);
        return 1;
    }
    fclose(fp);

    if (console_loglevel < 6) {
        fprintf(stderr, "[!] Loglevel < 6. nvmevirt logs might not appear.\n");
        fprintf(stderr, "    Try: echo 6 > /proc/sys/kernel/printk\n");
        return 1;
    }

    // 3. get last(tail) log
    char dmesg_line[1024];
    fp = popen(CMD_DMESG, "r");
    if (!fp || !fgets(dmesg_line, sizeof(dmesg_line), fp)) {
        fprintf(stderr, "[X] Failed to read dmesg nvmevirt line\n");
        if (fp) pclose(fp);
        return 1;
    }
    pclose(fp);

    double last_time = -1, uptime_now = -1;
    if (sscanf(dmesg_line, "[%lf]", &last_time) != 1) {
        fprintf(stderr, "[X] Failed to parse timestamp from dmesg\n");
        return 1;
    }

    // 4. read current system utime
    fp = fopen(UPTIME_PATH, "r");
    if (!fp || fscanf(fp, "%lf", &uptime_now) != 1) {
        fprintf(stderr, "[X] Failed to read /proc/uptime\n");
        if (fp) fclose(fp);
        return 1;
    }
    fclose(fp);

    double diff = uptime_now - last_time;
    if (diff > THRESHOLD_SEC) {
        printf("[!] No activity for %.0f sec. Consider unmounting %s\n", diff, mount_point);
    } else {
        printf("[*] Device active (%.0f sec since last I/O).\n", diff);
    }

    return 0;
}