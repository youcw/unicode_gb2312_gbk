#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define FILE_NAME       "test.txt"
#define FILE_NAME1      "test1.txt"

int main(int argc, char *argv[])
{
    int fd, fd1;
    struct stat st;
    char *buf;
    ssize_t ssize;
    char ch;
    int i, j;
    char tmp[1024];


    /* 打开文件test.txt*/
    fd = open(FILE_NAME, O_RDWR);
    if (fd < 0) { printf("open file %s failed\n", FILE_NAME); exit(-1);
    }
    
    /* 打开文件test1.txt*/
    fd1 = open(FILE_NAME1, O_CREAT|O_RDWR|O_EXCL|O_TRUNC,1000|777);
    if (fd1 < 0) { printf("open file %s failed\n", FILE_NAME1); exit(-1);
    }

    /* 获取文件属性*/
    if (fstat(fd, &st) == -1) {
        printf("stat file %s failed.\n", FILE_NAME);
        exit(-1);
    }
   
    printf("%s is %ld byte.\n", FILE_NAME, st.st_size);
    
    buf = malloc(st.st_size);
    if (buf == NULL) {
        printf("malloc failed.\n");
        exit(-1);
    }
   
    bzero(buf, 0);
    memset(tmp, 0, sizeof(tmp));

    /* 读文件到内存*/
    ssize = read(fd, buf, st.st_size);
    if (ssize == -1){
        printf("read %s failed.\n", FILE_NAME);
        exit(-1);
    }
   
    /* 字符处理*/
    for (i = 0, j = 0; i < ssize; ++i, ++j) {
        if (buf[i] == ',') { /* 判断是否为assic字符*/
            buf[i] = '|';
        } 
        
        if ((buf[i] & 0xff) > 0xbf) {   /* 判断是否为unicode*/
            if((((buf[i+2] & 0xff)<< 16)| ((buf[i+1] & 0xff) << 8) | (buf[i] & 0xff)) ==  0x8cbcef) { /* 判断是否是"，"*/
                buf[i] = '|';
                tmp[j] = buf[i];
                i += 2; /* 中文占3个字节，可以通过hexdump验证*/
            } else {
                memcpy(tmp+j, buf+i, 3);
            }
        } else
            tmp[j] = buf[i];
    }
   
    /* 处理结果写入文件*/
    if (write(fd1, tmp, strlen(tmp)) == -1) {
        printf("write %s failed.\n", FILE_NAME1);
        exit(-1);
    }
    
    close(fd);
    close(fd1);
    return 0;
}
