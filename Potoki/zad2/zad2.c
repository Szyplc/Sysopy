#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    double start, end, result, width = 0.000001;;
    if (scanf("%lf %lf", &start, &end) != 2) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    printf("Start: %f, Koniec: %f\n", start, end);
    if (mkfifo("/tmp/fifo1", 0666) == -1) {
        perror("mkfifo fifo1");
        return 1;
    }
    if (mkfifo("/tmp/fifo2", 0666) == -1) {
        perror("mkfifo fifo2");
        return 1;
    }

    int fd_out = open("/tmp/fifo1", O_WRONLY);
    write(fd_out, &start, sizeof(start));
    write(fd_out, &end, sizeof(start));
    write(fd_out, &width, sizeof(width));
    close(fd_out);

    int fd_in = open("/tmp/fifo2", O_RDONLY);
    read(fd_in, &result, sizeof(result));
    close(fd_in);

    unlink("/tmp/fifo1");
    unlink("/tmp/fifo2");
    printf("Value: %f\n", result);

    return 0;
}