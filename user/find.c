#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  // DIRSIZ is defined in fs.h,whose default value is 14;
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
//   printf("buf %s\n",buf);
  return buf;
}

// 需要找到递归的入口
void find(char *path,char *pattern)
{
    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // fstat is used to tell the fd is a dir,file or device;
    // write the state of the fd into st;
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    // If path is a file ,try to match the pattern;
    char str1[DIRSIZ+1],str2[DIRSIZ+1];
    strcpy(str1,fmtname(path));
    strcpy(str2,fmtname(pattern));
    // printf("%s %s\n",str1,str2); //this line is used to debug too;
    if(strcmp(str1,str2) == 0){
        // printf("%s %s\n",fmtname(path),fmtname(pattern)); // this line is used to debug;
        printf("%s\n", path);
    }
    break;

  case T_DIR:
  //If the path leads to a dir;
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    // copy the path to buffer;
    strcpy(buf, path);

    // append '/' to the buffer;
    p = buf+strlen(buf);
    *p++ = '/';

    // read the dir/file under this fd(path) into de(dirent);
    // Circulate to read;
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        // skip . and .. to avoid dead cycling;
      if(strcmp(de.name,".") == 0||strcmp(de.name,"..") == 0){
        continue;
      }
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      
      //go dfs;
    //   printf("%s \n",buf);
      find(buf,pattern);
      // we don't need to print the dir
    //   if(stat(buf, &st) < 0){
    //     printf("find: cannot stat %s\n", buf);
    //     continue;
    //   }
    //   printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 2||argc > 3){
    fprintf(2,"find: need at least 1 argument and at most 2 arguments\n");
    exit(1);
  }
  if(argc == 2){
    find(".",argv[1]);
  }else{
    find(argv[1],argv[2]);
  }
  exit(0);
}
