CC = gcc
CFLAGS = -Wall -g

SRCS = main.c log_util.c mount.c wait.c umount.c check_status.c
OBJS = $(SRCS:.c=.o)

TARGET = virt_manager

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c log_util.h mount.h umount.h wait.h check_status.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)