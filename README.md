# eleven
System Programming 2025 Team 11


virt_manager/

├── nvmevirt_logger.c          # insmod/mount/rmmod/umount 시 append log

├── nvmevirt_notify_client.c   # 사용자 예약 요청 및 대기

├── nvmevirt_notify_server.c   # 예약 큐 관리 + 알림 전송

├── watch_nvmevirt.c           # 비활성 사용자 감시 + 알림

├── nvmevirt.log               # log file

├── nvmevirt.queue             # 예약 대기열 파일

├── Makefile
