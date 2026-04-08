#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

int main(int argc, char *argv[]) {
    double start, end, result, width;
    int fd_in = open("/tmp/fifo1", O_RDONLY);
    read(fd_in, &start, sizeof(start));
    read(fd_in, &end, sizeof(start));
    read(fd_in, &width, sizeof(double));
    close(fd_in);
    result = 0;
    for(double i=start; i<end; i+=width) {
        result += f(i + 0.5 * width) * width;
    }
    int fd_out = open("/tmp/fifo2", O_WRONLY);
    write(fd_out, &result, sizeof(result));
    close(fd_out);
    
    return 0;
}