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
#include <pwd.h>
#include <grp.h>
#include <time.h>

//将st_mode的值转换成ls-al的权限形式
void getmod(int mode, char *mod) {
    //判断文件类型
    if (S_ISREG(mode)) {
        mod[0] = '-';
    } else if (S_ISDIR(mode)) {
        mod[0] = 'd';
    } else if (S_ISLNK(mode)) {
        mod[0] = 'l';
    } else if (S_ISCHR(mode)) {
        mod[0] = 'c';
    } else if (S_ISBLK(mode)) {
        mod[0] = 'b';
    } else if (S_ISFIFO(mode)) {
        mod[0] = 'p';
    } else {
        mod[0] = 's';
    }
    //判断各个用户的rwx权限
    if (mode & S_IRUSR) {
        mod[1] = 'r';
    } else {
        mod[1] = '-';
    }
    if (mode & S_IWUSR) {
        mod[2] = 'w';
    } else {
        mod[2] = '-';
    }
    if (mode & S_IXUSR) {
        mod[3] = 'x';
    } else {
        mod[3] = '-';
    }
    if (mode & S_IRGRP) {
        mod[4] = 'r';
    } else {
        mod[4] = '-';
    }
    if (mode & S_IWGRP) {
        mod[5] = 'w';
    } else {
        mod[5] = '-';
    }
    if (mode & S_IXGRP) {
        mod[6] = 'x';
    } else {
        mod[6] = '-';
    }
    if (mode & S_IROTH) {
        mod[7] = 'r';
    } else {
        mod[7] = '-';
    }
    if (mode & S_IWOTH) {
        mod[8] = 'w';
    } else {
        mod[8] = '-';
    }
    if (mode & S_IXOTH) {
        mod[9] = 'x';
    } else {
        mod[9] = '-';
    }
}

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
    struct passwd *usr;
    usr = getpwuid(file->st_uid);
    struct group *grp;
    grp = getgrgid(file->st_gid);
    struct tm *t = localtime(&file->st_mtime);
    char mod[11];
    getmod(file->st_mode, mod);
    printf("%s  ", mod);
    printf("%3d ", file->st_nlink);
    printf("%-15s ", usr->pw_name);
    printf("%-15s ", grp->gr_name);
    printf("%-20lld ", file->st_size);
    printf("%2d %2d %02d:%02d ", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
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
            if (i < argc - 1) {
                printf("\n");
            }
        }
    }
    return 0;
}
