#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stddef.h>

void filter(int read_fd){
    int prime,num;
    
    // read no data
    if(read(read_fd,&prime,sizeof(prime))==0){
        exit(0);
    }

    // the first num is prime
    printf("prime %d\n",prime);

    //start to filt
    int p[2];
    if(pipe(p) < 0){
        printf("Pipe creation failed");
        exit(1);
    }
    if(fork()==0){
        // child
        close(p[1]);
        filter(p[0]);
    }else{
        // this process passes the rest nums;
        while (read(read_fd,&num,sizeof(num))>0){
            if(num%prime!=0){
                write(p[1],&num,sizeof(num));
            }
        }
        // pipe is a buffer, close after all nums written;
        close(p[1]);
        wait(NULL);
    }
}

int main(int argc,char * args[]){
    int p[2];
    pipe(p);
    if(fork() == 0){
        //child
        close(p[1]);
        filter(p[0]);
    }else{
        //parent
        for(int i = 2;i<=35;i++){
            write(p[1],&i,4);
        }
        close(p[1]);
        wait(NULL);
    }
    return 0;
}