#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noreturn)) void send_sieved(int* p, int limit){
    int pp[2];
    int prime;
    int n;

    if(!read(p[0], &prime, sizeof(int))){
        exit(0);
    }

    printf("prime %d\n", prime);
    if(prime > limit/2){
        while(read(p[0], &prime, sizeof(int))){
            printf("prime %d\n", prime);
        }
        exit(0);
    }

    pipe(pp);


    while(read(p[0], &n, sizeof(int))){
        if(n % prime != 0){
            write(pp[1], &n, sizeof(int));
        }
    }

    close(p[0]);
    close(pp[1]);

    if(!fork()){ // child
        send_sieved(pp, limit);
    }
    else{ // parent
        wait(0);
        exit(0);
    }
}

void prime(int a, int b){
    int p[2];

    pipe(p);
    for(uint i = a; i <= b; i++){
        write(p[1], &i, sizeof(i));
    }

    close(p[1]);

    if(!fork()){ // child
        send_sieved(p, b);
    }
    else{ // parent
        wait(0);
        exit(0);
    }
}

int main(){
    prime(2, 500);

    exit(0);
}