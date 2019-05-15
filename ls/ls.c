/*************************************************************************
	> File Name: ls.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 六  5/11 14:42:54 2019
 ************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <string.h>
#include <grp.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define LEN 512

//0:ls
//1:ls -a
//2:ls -l
//3:ls -al

void mode_to_letters(mode_t);
void uid_to_name();
void gid_to_name();

int ls_opt = 0;
void do_ls(char*);
char work_dir[LEN];

int main(int argc, char *argv[]) {
    int opt;
    getcwd(work_dir, LEN);
    while((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt){
            case 'a':
                ls_opt++;
                break;
            case 'l':
                ls_opt += 2;
                break;
            default:
                exit(1);
        }
    }
    argc -= (optind - 1);
    argv += (optind - 1);
    printf("argv[%d] = %s\n", argc - 1, *(argv + 1));
    if (argc == 1) {
        do_ls(".");
    } else {
        while (--argc) {
            do_ls(*(++argv));
        }
    }
    return 0;
}

int cmp_name(const void* _a, const void* _b) {
    char *a = (char*)_a;
    char *b = (char*)_b;
    return strcmp(a, b);
}

void do_ls(char *dirname) {
    printf("%s\n", dirname);
    chdir(work_dir);
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    char file_name[1024][LEN] = {0};
    if ((dir_ptr = opendir(dirname)) == NULL) {
        perror("opendir");
    } else {
        int cnt = 0;
        while ((direntp = readdir(dir_ptr)) != NULL) {
            strcpy(file_name[cnt++], direntp->d_name);
            printf("%s\n", direntp->d_name);
        }
        qsort(file_name, cnt, LEN, cmp_name);
        for (int i = 0; i < cnt; i++) {
            printf("file_name[%d] = %s\n", i, file_name[i]);
        }
        if (ls_opt >= 2) {
            for (int i = 0; i < cnt; i++) {
                do_stat(file_name[i]);
            }
        }
    }
}

void do_stat(char *filename) {
    struct stat info;
    if (stat(filename, &info) == -1) {
        perror(filename);
    } else {
        show_file_info(filename, &info);
    }
}

void show_file_info(char *filename, struct stat *info) {
    char modestr[11] = "----------";
    mode_to_letters(info->stat_mode, modestr);
}

void mode_to_letters(mode_t mode, char *str) {
    if (S_ISDIR(mode)) str[0] = 'd';
    if (S_ISCHR(mode)) str[0] = 'c';
    if (S_ISBLK(mode)) str[0] = 'b';
    if (S_ISSOCK(mode)) str[0] = 's';
    if (S_ISFIFO(mode)) str[0] = 'p';
    if (S_ISLNK(mode)) str[0] = 'l';
    if (S_ISREG(mode)) str[0] = '-';

    if(mode & S_IRUSR) str[1] = 'r';
    if(mode & S_IWUSR) str[1] = 'w';
    if(mode & S_IXUSR) str[1] = 'x';

    if(mode & S_IRGRP) str[1] = 'r';
    if(mode & S_IWGRP) str[1] = 'w';
    if(mode & S_IXGRP) str[1] = 'x';

    if(mode & S_IROTH) str[1] = 'r';
    if(mode & S_IWOTH) str[1] = 'w';
    if(mode & S_IXOTH) str[1] = 'x';
}
