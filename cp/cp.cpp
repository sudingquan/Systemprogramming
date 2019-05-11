/*************************************************************************
	> File Name: cp.cpp
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 五  5/10 23:41:46 2019
 ************************************************************************/

#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

char argv0[100];

//判断是否为存在的目录
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

//判断是否是已经存在的文件
int is_exist_file(char *name) {
    FILE *fp;
    if ((fp = fopen(name, "r")) == NULL) {
        return 0;
    }
    fclose(fp);
    return 1;
}

//把一个文件从a复制到b
void file_copy(char *a, char *b) {
    //printf("copy %s to %s\n", a, b);
    FILE *fp1, *fp2;
    int bufferLen = 1024*4;  // 缓冲区长度
    char *buffer = (char*)malloc(bufferLen);//开辟缓存
    int readCount;  // 实际读取的字节数
    if ((fp1 = fopen(a, "rb")) == NULL) {
        fprintf(stderr, "%s: %s: No such file or directory\n", argv0, a);
        exit(EXIT_FAILURE);
    }
    if ((fp2 = fopen(b, "wb")) == NULL) {
        fprintf(stderr, "%s: %s: Permission denied\n", argv0, b);
        exit(EXIT_FAILURE);
    }
    while ((readCount = fread(buffer, 1, bufferLen, fp1)) > 0) {
        fwrite(buffer, readCount, 1, fp2);
    }
    free(buffer);
    fclose(fp1);
    fclose(fp2);
}

int main(int argc, char *argv[]) {
    int opt;
    int flag = 0;
    strcpy(argv0, argv[0]);
    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
            case 'a':
                flag = 1;
                break;
            default :
                fprintf(stderr, "usage: %s [-a] source_file target_file\n       %s [-a] source_file ... target_directory\n", argv0, argv0);
                exit(EXIT_FAILURE);
        }
    }
    argc -= optind;
    argv += optind;
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] : %s\n", i, argv[i]);
    }
    if (flag == 1) {
        if (argc < 2) {
            fprintf(stderr, "usage: %s [-a] source_file target_file\n       %s [-a] source_file ... target_directory\n", argv0, argv0);
            exit(EXIT_FAILURE);
        } else {
            if (ispath(argv[argc - 1])) {
                if (argc == 2) {
                    if (ispath(argv[0])) {
                        printf("copy exist_dir %s to exist_dir %s\n", argv[0], argv[1]);
                    } else {
                        if (is_exist_file(argv[0])) {
                            printf("copy exist_file %s to exist_dir %s\n", argv[0], argv[1]);
                        } else {
                            fprintf(stderr, "%s: %s: Not a directory\n", argv0, argv[0]);
                            exit(EXIT_FAILURE);
                        }
                    }
                } else {
                    printf("copy ...file_or_dir to exist_dir %s\n",argv[argc - 1]);
                }
              } else {
                if (argc == 2) {
                    if (ispath(argv[0])) {
                        if (is_exist_file(argv[1])) {
                            fprintf(stderr, "%s: %s: Not a directory\n", argv0, argv[1]);
                            exit(EXIT_FAILURE);
                        } else {
                            printf("copy exist_dir %s to not_etist_dir %s\n", argv[0], argv[1]);
                        }
                    } else {
                        file_copy(argv[0], argv[1]);
                    }
                } else {
                    fprintf(stderr, "usage: %s [-a] source_file target_file\n       %s [-a] source_file ... target_directory\n", argv0, argv0);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    return 0;
}
