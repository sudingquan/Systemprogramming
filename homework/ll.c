/*************************************************************************
	> File Name: ll.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 六  4/27 18:45:16 2019
 ************************************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

//判断输入的路径是目录还是文件
int ispath(char *path) {
    struct stat *st = malloc(sizeof(struct stat));
    stat(path, st);
    if (S_ISDIR(st->st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

//输出文件的各个信息
void showinfo(char *name) {
    struct stat *file = malloc(sizeof(struct stat));
    if (stat(name, file) < 0) {
        perror("stat");
        return;
    }
    printf("%d\t", file->st_mode);
    printf("%d\t", file->st_nlink);
    printf("%d\t", file->st_uid);
    printf("%d\t", file->st_gid);
    printf("%lld\t", file->st_size);
    printf("%s\n", name);
}
//输出指定文件夹的所有文件的信息
void showdirinfo(char *dirname) {
    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    char pwd[100];
    getcwd(pwd,100);
    chdir(dirname);
    printf("%s:\n", dirname);
    while ((entry = readdir(dir)) != NULL) {
        showinfo(entry->d_name);
    }
    chdir(pwd);
}

int main(int argc, char *argv[]) {
    //参数缺省时，默认为当前目录
    if (argc == 1) {
        showdirinfo(".");
    } else if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            if (ispath(argv[i])) {
                showdirinfo(argv[i]);
            } else {
                printf("%s:\n", argv[i]);
                showinfo(argv[i]);
            }
            printf("\n");
        }
    }
    return 0;
}
