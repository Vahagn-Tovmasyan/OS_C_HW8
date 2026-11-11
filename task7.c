#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    char firstPath[256], secondPath[256];
    unsigned char first[256], second[256];

    printf("Path to first file: ");
    fflush(stdout);

    if (!fgets(firstPath, sizeof(firstPath), stdin)) return 1;
    firstPath[strcspn(firstPath, "\n")] = '\0';

    printf("Path to second file: ");
    fflush(stdout);

    if (!fgets(secondPath, sizeof(secondPath), stdin)) return 1;
    secondPath[strcspn(secondPath, "\n")] = '\0';

    int a = open(firstPath, O_RDONLY);
    if (a < 0) {
        perror("open first");
        return 1;
    }

    int b = open(secondPath, O_RDONLY);
    if (b < 0) {
        perror("open second");
        close(a);
        return 1;
    }

    long long index = 0;
    int different = 0;

    while (!different) {
        ssize_t aCount = read(a, first, sizeof(first));
        ssize_t bCount = read(b, second, sizeof(second));

        if (aCount < 0 || bCount < 0) {
            perror("read");
            close(a);
            close(b);
            return 1;
        }

        ssize_t limit = aCount < bCount ? aCount : bCount;

        for (ssize_t i = 0; i < limit; i++) {
            if (first[i] != second[i]) {
                index += i;
                different = 1;
                break;
            }
        }

        if (different) break;

        index += limit;

        if (aCount != bCount) {
            different = 1;
            break;
        }

        if (aCount == 0) break;
    }

    if (different)
        printf("Files differ at byte %lld\n", index);
    else
        printf("Files are identical\n");

    if (close(a) < 0 || close(b) < 0) {
        perror("close");
        return 1;
    }

    return different ? 1 : 0;
}
