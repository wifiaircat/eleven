#ifndef MOUNT
#define MOUNT

void mount();
void write_log(const char *user, const char *status);

#endif
;

#ifndef WAIT
#define WAIT

void enqueue(const char *user);
int dequeue(char *next_user);

#endif
;

#ifndef UMOUNT
#define UMOUNT

void umount();
void notify_next_user();
void wait_for_notify(const char *user);
void write_log(const char *user, const char *status);

#endif