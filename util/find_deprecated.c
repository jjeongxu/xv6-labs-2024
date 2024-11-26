// only for "Lab Util: find". It's deprecated since it doesn't work with "Lab Util: xargs".
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
 
void strcat(char* str1, char* str2) {
    int str1Len = strlen(str1);
 
    for (int i = 0; i < strlen(str2); i++) {
        str1[str1Len + i] = str2[i];
    }
 
    str1[strlen(str1)] = 0;
}
 
__attribute__((noreturn)) void find(char* InitDir, char* FileName) {
    int fd_dir; // fd for Directory
    //int fd_spare = 0; // fd for a File
    char CurDir[512] = { 0, };
    struct stat st;
    struct dirent de;
 
    if ((fd_dir = open(InitDir, 0)) < 0) {
        fprintf(2, "[!] failed to open %s\n", InitDir);
        close(fd_dir);
        exit(1);
    }
 
    if (fstat(fd_dir, &st) < 0) {
        fprintf(2, "[!] fstat() failed\n");
        close(fd_dir);
        exit(1);
    }
 
    if (st.type != T_DIR) {
        fprintf(2, "[!] %s is not a directory\n", InitDir);
        fprintf(2, "Usage: find [target_dir] [file_name]\n");
        close(fd_dir);
        exit(1);
    }
 
    while (read(fd_dir, &de, sizeof(de)) == sizeof(de) && de.name[0]) {
        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;
 
        stat(de.name, &st);
        if(!strcmp(de.name, FileName) && st.type == T_FILE){
            printf("%s/%s\n", InitDir, FileName);
        }
 
        printf("de.name: %s\nst.type: %d\n", de.name, st.type);
 
        if (st.type == T_DIR) {
            if (!fork()) { // child
                strcpy(CurDir, InitDir);
                CurDir[strlen(CurDir)] = '/';
                strcat(CurDir, de.name);
 
                printf("[TEST] CurDir: %s\n", CurDir);
 
                find(CurDir, FileName);
            }
            else { // parent
                wait(0);
            }
        }
    }
 
    close(fd_dir);
    exit(0);
}
 
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find [target_dir] [file_name]\n");
        exit(1);
    }
 
    find(argv[1], argv[2]);
 
    exit(0);
}
