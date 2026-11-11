#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int file = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file < 0) {
        perror("open");
        return 1;
    }

    if (write(file, alphabet, 26) != 26) {
        perror("write");
        close(file);
        return 1;
    }
    if (close(file) < 0) {
        perror("close");
        return 1;
    }

    file = open("data.txt", O_RDWR);
    if (file < 0) {
        perror("reopen");
        return 1;
    }

    off_t before = lseek(file, 0, SEEK_END);
    if (before < 0) {
        perror("lseek");
        close(file);
        return 1;
    }
    printf("Size before: %lld\n", (long long)before);

    if (ftruncate(file, 10) < 0) {
        perror("ftruncate");
        close(file);
        return 1;
    }

    off_t after = lseek(file, 0, SEEK_END);
    if (after < 0 || lseek(file, 0, SEEK_SET) < 0) {
        perror("lseek");
        close(file);
        return 1;
    }
    printf("Size after: %lld\n", (long long)after);
    printf("Content: ");

    char letter;
    ssize_t n;
    while ((n = read(file, &letter, 1)) == 1) {
        putchar(letter);
    }
    if (n < 0) {
        perror("read");
        close(file);
        return 1;
    }
    putchar('\n');

    if (close(file) < 0) {
        perror("close");
        return 1;
    }
    return 0;
}
