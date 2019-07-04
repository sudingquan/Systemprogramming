/*************************************************************************
	> File Name: sum_file.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 四  7/ 4 18:06:58 2019
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
char sum_data[20] = "sum_data";
char now_data[20] = "now_data";

void get_num(char *filename, long long *num) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
	getline(&line, &len, fp);
    *num = atoi(line);
    free(line);
    fclose(fp);
}

void set_num(char *filename, long long num) {
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
	fprintf(fp, "%lld", num);
    fclose(fp);
}

int main() {
    pid_t pid;
    FILE *fp;
    int x = 0;
    long long sum = 0;
    long long num = 0;
    set_num(sum_data, sum);
    set_num(now_data, num);
    for (int i = 0; i < 10; i++) {
        pid = fork();
        if (pid == 0) {
            x = i;
            break;
        }
    }
    if (pid == 0) {
        while (1) {
            get_num(sum_data, &sum);
            get_num(now_data, &num);
            if (num > 100) break;
            sum += num;
            num++;
            set_num(sum_data, sum);
            set_num(now_data, num);
            printf("child process %d : %lld\n", x, sum);
        }
        exit(0);
    }
    int n = 0;
    while (n < 10) {
        waitpid(-1, NULL, 0);
        n++;
    }
    get_num(num, )
    return 0;
}
