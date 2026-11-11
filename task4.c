#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    char message[256];
    char entry[320];

    int file = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (file < 0) {
        perror("open");
        return 1;
    }

    printf("Type a log message: ");
    fflush(stdout);
    ssize_t count = read(STDIN_FILENO, message, sizeof(message) - 1);
    if (count < 0) {
        perror("read");
        close(file);
        return 1;
    }

    int length = 0;
    while (length < count && message[length] != '\n') length++;
    message[length] = '\0';

    int size = snprintf(entry, sizeof(entry), "PID=%ld: %s\n",
                        (long)getpid(), message);
    if (size < 0 || size >= (int)sizeof(entry)) {
        fprintf(stderr, "Message too long\n");
        close(file);
        return 1;
    }

    int written = 0;
    while (written < size) {
        ssize_t n = write(file, entry + written, size - written);
        if (n <= 0) {
            perror("write");
            close(file);
            return 1;
        }
        written += n;
    }

    // With O_APPEND, each write starts at the end of the file. After the
    // write, the descriptor's current position is the new end of the file.
    // Therefore lseek with SEEK_CUR returns an increasing offset.
    off_t end = lseek(file, 0, SEEK_CUR);
    if (end < 0) {
        perror("lseek");
        close(file);
        return 1;
    }
    printf("Offset after append: %lld\n", (long long)end);

    if (close(file) < 0) {
        perror("close");
        return 1;
    }
    return 0;
}
