/*************************************************************************
	> File Name: get_set_num.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 四  7/ 4 19:36:11 2019
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

void get_num(char *filename, int *num) {
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

void set_num(char *filename, int num) {
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
	fprintf(fp, "%d", num);
    fclose(fp);
}

int main() {
    int sum = 0;
    get_num("data", &sum);
    printf("sum = %d\n", sum);
    set_num("data", 321);
    get_num("data", &sum);
    printf("sum = %d\n", sum);
    return 0;
}

