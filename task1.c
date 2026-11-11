#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    char from[256], to[256], block[512];
    int input, output;
    ssize_t count;
    long long copied = 0;

    printf("Enter source path: ");
    fflush(stdout);
    if (!fgets(from, sizeof(from), stdin)) return 1;
    from[strcspn(from, "\n")] = '\0';

    printf("Enter destination path: ");
    fflush(stdout);
    if (!fgets(to, sizeof(to), stdin)) return 1;
    to[strcspn(to, "\n")] = '\0';

    if (from[0] == '\0' || to[0] == '\0' || strcmp(from, to) == 0) {
        fprintf(stderr, "Please enter two different file paths.\n");
        return 1;
    }

    input = open(from, O_RDONLY);
    if (input < 0) {
        perror("source open");
        return 1;
    }
    output = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output < 0) {
        perror("destination open");
        close(input);
        return 1;
    }

    while (1) {
        count = read(input, block, sizeof(block));
        if (count == 0) break;
        if (count < 0) {
            perror("read");
            close(input);
            close(output);
            return 1;
        }

        ssize_t done = 0;
        while (done < count) {
            ssize_t n = write(output, block + done, count - done);
            if (n <= 0) {
                perror("write");
                close(input);
                close(output);
                return 1;
            }
            done += n;
        }
        copied += count;
    }

    if (close(input) < 0) {
        perror("close source");
        close(output);
        return 1;
    }
    if (close(output) < 0) {
        perror("close destination");
        return 1;
    }

    printf("Copied %lld bytes.\n", copied);
    return 0;
}
