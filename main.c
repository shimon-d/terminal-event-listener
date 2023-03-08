#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>

void handle_event(int fd) {
    // Open a new terminal window when any event occurs
    system("gnome-terminal -e 'bash -c \"exec bash\"'");
}

int main(int argc, char* argv[]) {
    int fd, wd;
    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    struct inotify_event *event;
    ssize_t len;
    char *ptr;
    int counter = 0;
    int limiter = 2;

    // Create a new inotify instance
    fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Add the directory to watch
    wd = inotify_add_watch(fd, "/dev/pts", IN_ALL_EVENTS);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    // Read events from the inotify file descriptor
    for (;;) {
        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (struct inotify_event *) ptr;
            handle_event(fd);
        }

        counter++;
        if (counter == limiter) {
            break;
        }
    }

    exit(EXIT_SUCCESS);
}
