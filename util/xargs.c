#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
 
#define MAXLEN 512
 
char CutStr(char* InitStr, char** ArgArr, char TargetChar){
    char* StartPtr;
    char* EndPtr;
    int Idx = 0;
 
    StartPtr = InitStr;
    for(int i = 0; InitStr[i] != 0; i++){
        if(InitStr[i] == TargetChar){
            EndPtr = InitStr+i;
            ArgArr[Idx] = (char*)malloc(EndPtr-StartPtr+1); // added 1, for NULL
            memset(ArgArr[Idx], 0x0, EndPtr-StartPtr+1+1);
            for(int j = 0; j < EndPtr-StartPtr; j++){
                ArgArr[Idx][j] = StartPtr[j];
            }
            Idx++;
            StartPtr = EndPtr+1;
        }
 
        if(Idx==33){
            fprintf(2, "[!] Count of args is larger than MAXARG(32)\n");
            return 0;
        }
    }
 
    return 1;
}
 
int main(int argc, char* argv[]){
    char ExitStatus = 0;
    char Buf[2048] = {0,}; // Init String(left side of the pipe's result) 
    char* LeftArg[MAXARG]; // left side of the pipe's string will be cutted then, stored.
    char* XargsArg[MAXARG]; // Args for Xargs will be stored
 
    memset(LeftArg, 0x0, sizeof(char*)*MAXARG);
    memset(XargsArg, 0x0, sizeof(char*)*MAXARG);
 
    read(0, Buf, 2048);
 
    if(Buf[2048-1]){
        fprintf(2, "[!] left result's length is larger than 2048\n");
        exit(1);
    }
 
    if(!CutStr(Buf, LeftArg, '\n')){
        ExitStatus = 1;
        goto clean_up;
    }
 
    for(int i = 1; argv[i] != 0; i++){
        XargsArg[i-1] = argv[i];
    }
 
    for(int i = 0; LeftArg[i] != 0; i++){
        XargsArg[argc-1] = LeftArg[i];
        if(!fork()){ // child
            exec(argv[1], XargsArg);
        }
        else{ // parent
            wait(0);
        }
    }
 
    clean_up:
    for(int i = 0; LeftArg[i] != 0; i++){
        free(LeftArg[i]);
    }
 
    exit(ExitStatus);
}
