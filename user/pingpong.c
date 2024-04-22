#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main (int argc,char * args[]){
    // the file descriptors of the pipe;
    // remember p[0] is the read end,rather p[1] is the write end;
    // If you mix them, the read end won't block;
    int p1[2];
    int p2[2];
    // the buffer to store data;
    char buf[512];
    // create pipeline;
    pipe(p1);
    pipe(p2);
    // start the child process;
    if(fork() == 0){
        // child
        read(p1[0],buf,1);
        close(p1[0]);
        printf("%d: received ping\n",getpid());
        write(p2[1],"pong",1);
        close(p2[1]);
    }else{
        write(p1[1],"ping",1);
        close(p1[1]);
        read(p2[0],buf,1);
        close(p2[0]);
        printf("%d: received pong\n",getpid());
    }
    exit(0);
}