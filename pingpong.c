#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    int p[2];
    char buf[10] = {0,};

    pipe(p);
    if(fork() > 0) { // parent side
        write(p[1], "ping\0", 5);
        wait(0);
        if(read(p[0], buf, 5) != 5){
            fprintf(2, "%d(Parent): Error occurred while reading\n", getpid());
            exit(1);
        }
        printf("%d: received %s\n", getpid(), buf);

        close(p[0]);
        close(p[1]);

        exit(0);
    }
    else { // child side
        if(read(p[0], buf, 5) != 5){
            fprintf(2, "%d(Child): Error occurred while reading\n", getpid());
            exit(1);
        }
        printf("%d: received %s\n", getpid(), buf);
        write(p[1], "pong\0", 5);

        close(p[0]);
        close(p[1]);
        exit(0);
    }
}