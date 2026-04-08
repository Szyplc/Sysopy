#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

double calculate_integral(double width, int n, double (*f)(double)) {
    int m = (int)(1.0 / width);
    double sum = 0;
    pid_t pid;
    int fd[n][2];
    
    for(int k=0; k<n; k++) {
        pipe(fd[k]);
        pid = fork();
        if (pid == 0) {
            close(fd[k][0]);
            sum = 0;
            for(int i=k; i<m; i+=n) {
                sum += f((i + 0.5) * width) * width;
            }
            write(fd[k][1], &sum, sizeof(sum));
            close(fd[k][1]);
            exit(0);
        } else {
            close(fd[k][1]);
        }
    }

    for(int k=0; k<n; k++) {
        double value;
        read(fd[k][0], &value, sizeof(value));
        sum += value;
        close(fd[k][0]);
    }

    for(int k=0; k<n; k++) {
        wait(NULL);
    }

    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <szerokość> <n>\n", argv[0]);
        return 1;
    }

    double width = atof(argv[1]);
    int n = atoi(argv[2]);

    if (width <= 0 || n <= 0) {
        fprintf(stderr, "Błąd: szerokość i n muszą być > 0\n");
        return 1;
    }

    for(int i=1; i<=n; i++) {
        struct timeval start, end;
        gettimeofday(&start, NULL);

        double result = calculate_integral(width, i, f);

        gettimeofday(&end, NULL);
        double elapsed = (end.tv_sec - start.tv_sec) +
                        (end.tv_usec - start.tv_usec) / 1000000.0;

        printf("Wynik: %f, dla i: %d\n", result, i);
        printf("Czas obliczen: %.6f s\n", elapsed);
    }

    return 0;
}