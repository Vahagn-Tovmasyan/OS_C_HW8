#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const char initial[] = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n";
    char remaining[100];
    char ch;

    int file = open("numbers.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file < 0) {
        perror("open");
        return 1;
    }

    if (write(file, initial, sizeof(initial) - 1) != sizeof(initial) - 1) {
        perror("write");
        close(file);
        return 1;
    }

    if (close(file) < 0) {
        perror("close");
        return 1;
    }

    file = open("numbers.txt", O_RDWR);
    if (file < 0) {
        perror("reopen");
        return 1;
    }

    int lines = 0;
    while (lines < 3) {
        ssize_t n = read(file, &ch, 1);

        if (n != 1) {
            if (n < 0) perror("read");
            else fprintf(stderr, "Line 4 not found\n");
            close(file);
            return 1;
        }

        if (ch == '\n') lines++;
    }

    off_t fourth = lseek(file, 0, SEEK_CUR);
    if (fourth < 0) {
        perror("lseek");
        close(file);
        return 1;
    }

    // Skip the fourth line to save the content after it.
    do {
        ssize_t n = read(file, &ch, 1);
        if (n != 1) {
            if (n < 0) perror("read");
            else fprintf(stderr, "Fourth line is incomplete\n");
            close(file);
            return 1;
        }
    } while (ch != '\n');

    int tailLength = 0;
    ssize_t n;

    while ((n = read(file, remaining + tailLength,
                     sizeof(remaining) - tailLength)) > 0) {
        tailLength += n;

        if (tailLength == (int)sizeof(remaining)) {
            fprintf(stderr, "Remaining content is too large\n");
            close(file);
            return 1;
        }
    }

    if (n < 0) {
        perror("read");
        close(file);
        return 1;
    }

    if (lseek(file, fourth, SEEK_SET) < 0) {
        perror("lseek");
        close(file);
        return 1;
    }

    if (write(file, "100\n", 4) != 4 ||
        write(file, remaining, tailLength) != tailLength) {
        perror("write");
        close(file);
        return 1;
    }

    off_t end = lseek(file, 0, SEEK_CUR);

    if (end < 0 || ftruncate(file, end) < 0 ||
        lseek(file, 0, SEEK_SET) < 0) {
        perror("lseek/ftruncate");
        close(file);
        return 1;
    }

    printf("Updated numbers:\n");
    fflush(stdout);

    char block[64];

    while ((n = read(file, block, sizeof(block))) > 0) {
        ssize_t pos = 0;

        while (pos < n) {
            ssize_t sent = write(STDOUT_FILENO, block + pos, n - pos);

            if (sent <= 0) {
                perror("write stdout");
                close(file);
                return 1;
            }

            pos += sent;
        }
    }

    if (n < 0) {
        perror("read");
        close(file);
        return 1;
    }

    if (close(file) < 0) {
        perror("close");
        return 1;
    }

    return 0;
}
