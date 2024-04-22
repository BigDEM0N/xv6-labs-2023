#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// #include "user/ulib.c"
// warning: you shouldn't includ ulib.c for there would be two "main" in the program.
// If you tend to use the functions in ulib.c,you need include user.h whitch illustrate the functions.

int
main (int argc,char * args[]){
    if(argc!=2){
        printf("Usage: sleep [time]");
        exit(1);
    }
    int time = atoi(args[1]);
    sleep(time);
    printf("nothing happens for a little while\n");
    exit(0);
}