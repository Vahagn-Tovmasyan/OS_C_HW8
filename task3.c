#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    char filename[256];
    char byte;

    printf("Enter a text file: ");
    fflush(stdout);
    if (!fgets(filename, sizeof(filename), stdin)) return 1;
    filename[strcspn(filename, "\n")] = '\0';

    int file = open(filename, O_RDONLY);
    if (file < 0) {
        perror("open");
        return 1;
    }

    off_t length = lseek(file, 0, SEEK_END);
    if (length < 0) {
        perror("lseek");
        close(file);
        return 1;
    }

    off_t distance = 1;
    while (distance <= length) {
        if (lseek(file, -distance, SEEK_END) < 0) {
            perror("lseek");
            close(file);
            return 1;
        }
        ssize_t n = read(file, &byte, 1);
        if (n != 1) {
            if (n < 0) perror("read");
            else fprintf(stderr, "Unexpected end of file\n");
            close(file);
            return 1;
        }
        if (write(STDOUT_FILENO, &byte, 1) != 1) {
            perror("write");
            close(file);
            return 1;
        }
        distance++;
    }
    if (write(STDOUT_FILENO, "\n", 1) != 1) {
        perror("write");
        close(file);
        return 1;
    }
    if (close(file) < 0) {
        perror("close");
        return 1;
    }
    return 0;
}
