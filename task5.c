#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const int gap = 1024 * 1024;
    int file = open("sparse.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file < 0) {
        perror("open");
        return 1;
    }

    if (write(file, "START", 5) != 5) {
        perror("write START");
        close(file);
        return 1;
    }
    if (lseek(file, gap, SEEK_CUR) < 0) {
        perror("lseek");
        close(file);
        return 1;
    }
    if (write(file, "END", 3) != 3) {
        perror("write END");
        close(file);
        return 1;
    }

    // lseek skips 1 MiB without storing zeros. This creates a hole in the
    // file, so du can show much less disk usage than the apparent size.
    if (close(file) < 0) {
        perror("close");
        return 1;
    }

    file = open("sparse.bin", O_RDONLY);
    if (file < 0) {
        perror("reopen");
        return 1;
    }
    off_t length = lseek(file, 0, SEEK_END);
    if (length < 0) {
        perror("lseek");
        close(file);
        return 1;
    }
    printf("File size: %lld bytes\n", (long long)length);
    if (close(file) < 0) {
        perror("close");
        return 1;
    }
    return 0;
}
