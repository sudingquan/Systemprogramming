/*************************************************************************
	> File Name: ls.cpp
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
#include <string.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <sys/ioctl.h>
#include <math.h>
#define MAX_N 10000

using namespace std;

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
    struct stat *st = (struct stat *)malloc(sizeof(struct stat));
    stat(path, st);
    if (S_ISDIR(st->st_mode)) {
        return 1;
    } else {
        return 0;
    }
    free(st);
}
//输出文件的各个信息
void showinfo(char *name) {
    struct stat *file = (struct stat *)malloc(sizeof(struct stat));
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
    printf("%20lld ", file->st_size);
    printf("%2d月 %2d日 %02d:%02d ", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
    printf("%s\n", name);
    free(file);
}

//输出指定目录的所有文件的信息
void showdirinfo(char *dirname) {
    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    char pwd[100];
    string fileName[MAX_N];
    getcwd(pwd, 100);
    chdir(dirname);
    printf("%s:\n", dirname);
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        fileName[i] = entry->d_name;
        i++;
    }
    sort(fileName, fileName + i);
    for (int j = 0; j < i; j++) {
        char cfileName[200];
        strcpy(cfileName, fileName[j].c_str());
        showinfo(cfileName);
    }
    chdir(pwd);
}

//输出文件名称
void showname(char *name) {
    struct stat *file = (struct stat *)malloc(sizeof(struct stat));
    if (stat(name, file) < 0) {
        perror("stat");
        return;
    }
    printf("%s\n", name);
}

//得到指定目录的非隐藏文件名称
void getdirname(char *dirname, string fileName[MAX_N],int *sum){
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
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (!strncmp(entry->d_name, ".", 1)) {
            continue;
        }
        fileName[i] = entry->d_name;
        i++;
    }
    chdir(pwd);
    *sum = i;
    sort(fileName, fileName + i);
}

//得到一列中文件名中最长的一个文件名的长度
int getnamelen(string *fileName, int sum) {
    int len = 0;
    for (int i = 0; i < sum; i++) {
        if ((fileName + i)->size() > len) {
            len = (fileName + i)->size();
        }
    }
    return len;
}

//输出文件名称
void showdirname(char *dirname) {
   // char **fileName = (char **)malloc(sizeof(char *) * 10000);
   // for (int i = 0; i < 10000; i++) {
   //     fileName[i] = (char*)malloc(sizeof(char) * 100);
   // }
    string fileName[MAX_N];
    int sum = 0;    
    getdirname(dirname, fileName, &sum);
    int max_list = sum;
    int col = 1;
    int line;
    int last_line;
    struct winsize size;
    if (isatty(STDOUT_FILENO) == 0)
        exit(1);
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &size)<0)
    {
        perror("ioctl TIOCGWINSZ error");
        exit(1);
    }
    //printf("%d rows, %d columns\n", size.ws_row, size.ws_col);
    for (int i = 2; i <= ceil(max_list / 2.0); i++) {
        int width = 0;
        int sum_width = 0;
        line = sum / i;
        last_line = sum % i;
        for (int j = 0; j < max_list; j++) {
            sum_width += getnamelen(fileName + j, 1);
        }
        for (int j = 0; j < i; j++) {
            if (j == i - 1) {
                width += getnamelen(fileName + j * line, last_line);
            } else {
                width += getnamelen(fileName + j * line, line);
            }
        }
        width += i;
        if (line == 1 || i == ceil(max_list / 2.0)) {
            if (sum_width < size.ws_col) {
                col = max_list;
                line = ceil((double)max_list / col);
                break;
            } else {
                col = ceil(max_list / 2.0);
                line = ceil((double)max_list / col);
                break;
            }
        }
        if (width > size.ws_col) {
            col = i - 1;
            line = ceil((double)max_list / col);
            break;
        }
       // if (i == ceil(max_list / 2.0)) {
       //     col = max_list;
       //     line = ceil((double)max_list / col);
       //     printf("if if\n");
       //     break;
       // }
    }
    cout << "col:"  << col << "\t";
    cout << "line:"  << line << endl;
    int *col_longest = new int [col];
    for (int i = 0; i < col; i++) {
        if (i == col - 1) {
            col_longest[i] = getnamelen(fileName + i * line,last_line);
        } else {
            col_longest[i] = getnamelen(fileName + i * line, line);
        }
    }
    for (int i = 0; i < line; i++) {
        for (int j = 0 ; j < col; j++) {
            cout << fileName[i + j * line];
            int name_size = fileName[i + j  *  line].size();
            for (int k = 0; k <= col_longest[j] - name_size;  k++) {
                cout << " ";
            }
        }
        cout << endl;
    }
    delete[] col_longest;
}

int main(int argc, char *argv[]) {
    //参数缺省时，默认为当前目录
    //选项缺省时默认为ls
    char point[] = ".";
    if (argc == 1) {
        showdirname(point);
    } else if (argc >= 2) {
        if (!strncmp(argv[1], "-", 1)) {
            char option[10];
            strncpy(option, argv[1] + 1, strlen(argv[1]) - 1);
            option[strlen(argv[1]) - 1] = '\0';
            if (strlen(option) == 2 && strncmp(option, "al", 2) == 0) {
                if (argc == 2) {
                    showdirinfo(point);
                } else {
                    for (int i = 2; i < argc; i++) {
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
            } else {
                printf("%s: illegal option -- %s\n", argv[0], option);
            }
        } else {
            for (int i = 1; i < argc; i++) {
                if (ispath(argv[i])) {
                    showdirname(argv[i]);
                } else {
                    showname(argv[i]);
                }
                if (i < argc - 1) {
                    printf("\n");
                }
            }
        }
    }
    return 0;
}
