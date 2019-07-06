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
#include <sys/file.h>
#define MAX 10000
#define PROCESS_NUM 20

char sum_data[20] = "sum_data";
char now_data[20] = "now_data";

void get_num(long long *num, FILE *fp) {
    char *line = NULL;
    size_t len = 0;
	getline(&line, &len, fp);
    fseek(fp, 0, 0);
    *num = atoll(line);
    free(line);
}

void set_num(long long num, FILE *fp) {
	fprintf(fp, "%lld", num);
}

int main() {
    pid_t pid;
    int x = 0;
    FILE *fp1 = fopen(sum_data, "w");
    FILE *fp2 = fopen(now_data, "w");
    if (fp1 == NULL) {
        perror("fopen");
        exit(1);
    }
    if (fp2 == NULL) {
        perror("fopen");
        exit(1);
    }
    set_num(0, fp1);
    set_num(0, fp2);
    fclose(fp1);
    fclose(fp2);
    for (int i = 0; i < PROCESS_NUM; i++) {
        pid = fork();
        if (pid == 0) {
            x = i;
            break;
        }
    }
    if (pid == 0) {
        while (1) {
            long long sum = 0;
            long long num = 0;
            FILE *fp1 = fopen(sum_data, "r+");
            FILE *fp2 = fopen(now_data, "r+");
            if (fp1 == NULL) {
                perror("fopen");
                exit(1);
            }
            if (fp2 == NULL) {
                perror("fopen");
                exit(1);
            }
            flock(fileno(fp1), LOCK_EX);
            flock(fileno(fp2), LOCK_EX);
            get_num(&sum, fp1);
            get_num(&num, fp2);
            if (num > MAX) {
                flock(fileno(fp1), LOCK_UN);
                flock(fileno(fp2), LOCK_UN); 
                break;
            }
            sum += num;
            num++;
            set_num(sum, fp1);
            set_num(num, fp2);
            printf("child process %d now : %lld\n", x, num);
            printf("child process %d sum : %lld\n", x, sum);
            fclose(fp1);
            fclose(fp2);
        }
        exit(0);
    }
    int n = 10;
    while (n--) {
        waitpid(-1, NULL, 0);
    }
    long long sum;
    FILE *fp = fopen(sum_data, "r+");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    get_num(&sum, fp);
    printf("sum = %lld\n", sum);
    return 0;
}
