// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
int match(char*, char*);

void
grep(char *pattern, int fd)
{
  int n, m;
  char *p, *q;

  m = 0;
  // buf+m append to the buffer，read max to the size of buffer-1
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
    // calculate the offset in buffer
    m += n;
    // append the EOF
    buf[m] = '\0';
    p = buf;
    // strchr is used to find a specific char int string p
    // q = strchr(p,'\n') means to find if the user pressed 'Enter'.If so,the input is ended,or else skip
    while((q = strchr(p, '\n')) != 0){
      //let the '\n' in p be 0?
      *q = 0;
      if(match(pattern, p)){
        *q = '\n';
        // if match, write buffer[0]~buffer[q-p] to console
        write(1, p, q+1 - p);
      }
      //let the point be the buffer[q+1],whitch means to read next input;
      p = q+1;
    }
    // m represents the char numbers in buffer.
    // So after we use the first q-p+1 char to match,we need to slide the data behind;
    if(m > 0){
      // we have m chars left;
      m -= p - buf;
      // copy them to buffer[0];
      memmove(buf, p, m);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  char *pattern;

  // If we have only one arg,it means wrong input;
  if(argc <= 1){
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  // If we have more than one arg,we have the second arg as pattern;
  // ps: the first arg is "grep";
  pattern = argv[1];

  // If we have only two args,we do one time grep;
  if(argc <= 2){
    grep(pattern, 0);
    exit(0);
  }

  // let the pattern match to the other args ,not string but maybe file?;
  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("grep: cannot open %s\n", argv[i]);
      exit(1);
    }
    grep(pattern, fd);
    close(fd);
  }
  exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

