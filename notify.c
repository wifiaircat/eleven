#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "log_util.h" // write_log()가 존재하는 헤더 파일

#define CURRENT_FILE "current_status.txt"
#define QUEUE_FILE "queue.txt"
#define SOCKET_FMT "/tmp/notify_%s.sock"
#define DEVICE "/dev/nvme0n1"
#define MOUNT_DIR "/home/aircat/nvmevirt/mnt"

// 외부 로그 기록 함수
extern void write_log(const char *user, const char *status);

// 현재 사용자 상태 제거 (current_status.txt 초기화)
void clear_status() {
    FILE *fp = fopen(CURRENT_FILE, "w");
    if (fp) {
        // 파일을 비워 "현재 사용자 없음" 상태로 전환
        fprintf(fp, "none\n");
        fclose(fp);
    }
}

// 대기열에서 다음 사용자 pop (제거하고 next_user에 반환)
int dequeue(char *next_user) {
    FILE *fp = fopen(QUEUE_FILE, "r");
    if (!fp) return 0;

    FILE *tmp = fopen("queue_tmp.txt", "w");
    int found = 0;
    char line[64];

    while (fgets(line, sizeof(line), fp)) {
        if (!found) {
            sscanf(line, "%s", next_user);  // 첫 번째 사용자만 추출
            found = 1;
        } else {
            fputs(line, tmp);  // 나머지는 임시 파일에 저장
        }
    }

    fclose(fp);
    fclose(tmp);

    // 원본 queue.txt 삭제 → tmp로 대체
    remove(QUEUE_FILE);
    rename("queue_tmp.txt", QUEUE_FILE);
    return found;
}

// 다음 사용자에게 Unix 도메인 소켓으로 알림 전송
void notify_next_user(const char *admin_user) {
    char next_user[64];
    
    if (!dequeue(next_user)) {
        // 대기자가 없다면 로그만 남기고 종료
        write_log(admin_user, "no waiting user");
        return;
    }

    // 소켓 경로 구성 (/tmp/notify_<username>.sock)
    char sock_path[108];
    snprintf(sock_path, sizeof(sock_path), SOCKET_FMT, next_user);

    // 소켓 생성 (UDP 방식)
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        write_log(admin_user, "socket creation failed");
        return;
    }

    // 수신자 주소 설정
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    // 메시지 전송
    if (sendto(sock, "go", 2, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("notify failed");
        write_log(admin_user, "notify failed to user");
    } else {
        write_log(admin_user, "notify success to next user");
    }

    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <your_username>\n", argv[0]);
        return 1;
    }

    const char *user = argv[1];

    // 1. 장치 반납
    printf("Unmounting and removing kernel module...\n");
    int ret = system("sudo umount /dev/nvme0n1 && sudo rmmod nvmev");

    if (ret != 0) {
        write_log(user, "umount + rmmod failed");
        printf("Failed to release device.\n");
        return 1;
    }

    write_log(user, "umount + rmmod success");

    // 2. 현재 사용자 정보 초기화
    clear_status();

    // 3. 다음 사용자 알림
    notify_next_user(user);

    printf("Device released. If queue exists, next user has been notified.\n");

    return 0;
}

