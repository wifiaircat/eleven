#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CMD_CUR_USER "tail -n 1 /proc/mounts"
#define CMD_DMESG "sudo dmesg | grep nvmevirt | tail -n 1"
#define LOGLEVEL_PATH "/proc/sys/kernel/printk"
#define UPTIME_PATH "/proc/uptime"
#define INTERVAL_SEC 3000

int main() {
    // 1. who is current user
        FILE *fp = popen(CMD_CUR_USER, "r");
        if (!fp) { perror("fail to popen"); }

        char cur_usr_buffer[256];
        if (!fgets(cur_usr_buffer, sizeof(cur_usr_buffer), fp)) {
            fprintf(stderr, "[F] Failed to read /proc/mounts\n");
            pclose(fp); return 1;
        }
        pclose(fp);

        char *device = strtok(cur_usr_buffer, " ");
        char *mount_point = strtok(NULL, " ");
        printf("[*] Checking device: %s mounted on %s\n", device, mount_point);

        // 2. check kernel log lv
        int console_loglevel = -1;
        fp = fopen(LOGLEVEL_PATH, "r");
        if (!fp || fscanf(fp, "%d", &console_loglevel) != 1) {
            fprintf(stderr, "[F] Failed to read loglevel\n");
        }
        fclose(fp);

        if (console_loglevel < 6) {
            fprintf(stderr, "[!] Loglevel %d is < 6. nvmevirt logs might not appear.\n", console_loglevel);
            fprintf(stderr, "    Try: echo 6 > /proc/sys/kernel/printk\n");
        }

    while(1){
        // 3. get last(tail) log
        char dmesg_line[1024];
        fp = popen(CMD_DMESG, "r");
        if (!fp) {
            perror("[X] popen for CMD_DMESG failed");
            sleep(10);
            continue;
        }

        if (!fgets(dmesg_line, sizeof(dmesg_line), fp)) {
            fprintf(stderr, "[X] Failed to read dmesg nvmevirt line\n");
            if(fp) fclose(fp);
            sleep(10);
            continue;
        }
        fclose(fp);

        double last_time = -1, uptime_now = -1;
        if (sscanf(dmesg_line, "[%lf]", &last_time) != 1) {
            fprintf(stderr, "[X] Failed to parse timestamp from dmesg\n");
            sleep(10);
            continue;
        }

        // 4. read current system utime
        fp = fopen(UPTIME_PATH, "r");
        if (!fp || fscanf(fp, "%lf", &uptime_now) != 1) {
            fprintf(stderr, "[X] Failed to read /proc/uptime\n");
            if(fp) fclose(fp);
            sleep(10);
            continue;
        }
        fclose(fp);

        double diff_sec = uptime_now - last_time;
        double diff_min = diff_sec / 60;
        if (diff_sec > INTERVAL_SEC) {
            printf("[!] No activity for %.1fmin. Consider unmounting %s\n", diff_min, mount_point);
        } else {
            printf("[*] Device active (%.1fmin since last I/O).\n", diff_min);
        }

        printf("[*] Sleeping for %dmin...\n", INTERVAL_SEC / 60);
        sleep(INTERVAL_SEC);
    }

    return 0;
}