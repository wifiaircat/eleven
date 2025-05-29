#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STAT_PATH "/sys/block/nvmev0n1/stat"
#define SLEEP_SEC 30
#define STAT_LINE_LEN 256

int read_stat(char *buf, size_t size) {
    FILE *fp = fopen(STAT_PATH, "r");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    if (!fgets(buf, size, fp)) {
        perror("fgets");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int main() {
    char stat_before[STAT_LINE_LEN];
    char stat_after[STAT_LINE_LEN];

    // stat 파일 존재 여부 확인
    if (access(STAT_PATH, F_OK) != 0) {
        fprintf(stderr, "[X] %s file dosen't exits.\n", STAT_PATH);
        return 1;
    }

    // 처음 stat 값 읽기
    if (read_stat(stat_before, sizeof(stat_before)) != 0) {
        fprintf(stderr, "[X] failed to read 1st stat file\n");
        return 1;
    }

    // 대기
    sleep(SLEEP_SEC);

    // 두 번째 stat 값 읽기
    if (read_stat(stat_after, sizeof(stat_after)) != 0) {
        fprintf(stderr, "[X] failed to read 2nd stat file.\n");
        return 1;
    }

    // 비교
    if (strcmp(stat_before, stat_after) == 0) {
        printf("[!] %s: %dsec no move.\n", STAT_PATH, SLEEP_SEC);
    } else {
        printf("[✓] %s: active.\n", STAT_PATH);
    }

    return 0;
}
