#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>
#include "util.h"

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_BLOCK_LINES 32
#define MAX_VARS 64
#define MAX_JOBS 32

typedef struct {
    pid_t pid;
    char command[MAX_LINE];
    int stopped;
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;
pid_t fg_pid = -1;

int execute_external_command(char **args, int background){
    int input_fd = -1, output_fd = -1;
    char *clean_args[MAX_ARGS];
    int j = 0;

    for (int i = 0; args[i] != NULL; i++){
        if (strcmp(args[i], "<") == 0 && args[i+1]){
            input_fd = open(args[i+1], O_RDONLY);
            if(input_fd < 0){
                perror("open for input");
                return -1;
            }
            i++;
        } else if (strcmp(args[i], ">") == 0 && args[i+1]){
            output_fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0){
                perror("open for output");
                return -1;
            }
            i++;
        } else {
            clean_args[j++] = args[i];
        }
    }
    clean_args[j] = NULL;

    pid_t pid = fork();
    if (pid < 0){
        perror("fork");
        return -1;
    } else if (pid==0){ //child
        if (input_fd != -1){
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != -1) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        execvp(clean_args[0], clean_args);
        fprintf(stderr, "%s: command not found\n", clean_args[0]);
        exit(1);
    } else {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, clean_args[0], MAX_LINE);
        if (background){
            jobs[job_count].stopped = 0;
            job_count++;
            printf("[background pid %d]\n", pid);
        } else {
            int status;
            fg_pid = pid;
            waitpid(pid, &status, WUNTRACED);
            if (WIFSTOPPED(status)){
                jobs[job_count].stopped = 1;
                printf("\n[Stopped] pid %d\n", pid);
            } else {
                jobs[job_count].stopped = 0;
            }
            job_count++;
            fg_pid = -1;
            return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        }
    }
}

void execute_command(char** args) {
    if (args[0] == NULL) return;

    int i = 0;
    int background = 0;
    while (args[i]) i++;
    if (i > 0 && strcmp(args[i-1], "&") == 0){
        background = 1;
        args[i-1] = NULL;
    }

    if (strcmp(args[0], "mount") == 0){
        //if mount success
        execute_external_command(args, background);
    }

    if (strcmp(args[0], "wait") == 0){
        exit(0);
    }

    if (strcmp(args[0], "umount") == 0){
        exit(0);
    }

    if (strcmp(args[0], "exit") == 0){
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;
        execute_command(line);
    }
    return 0;
}