/*!
*   @file tcpserver.cpp
*
*   This is part of the Kria Kitchen Lifeguard Project
*  TCP socket server, retrieves and publishes the last
*  data published by the people detector. 
*
*
*   Written by Enrique Albertos, with contributions from the 
*   open source community and Xilinx libraries.
*   Created: 2022-03-29
*   Public Domain
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include <fcntl.h>

#define FileName "data.dat"
#define BUF_SIZE 8192

void report_and_exit(const char* msg);
int readData(char* buffer, int bufferSize);

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        ticks = time(NULL);
        memset(sendBuff, '0', sizeof(sendBuff));
        int readLenght = readData(sendBuff, sizeof(sendBuff));
        write(connfd, sendBuff, strlen(sendBuff));
        write(connfd, "\n", 1);
        close(connfd);
        sleep(1);
     }
}



void report_and_exit(const char* msg) {
  perror(msg);
}

int readData(char* buffer, int bufferSize) {
  
  struct flock lock;
  lock.l_type = F_WRLCK;    /* read/write (exclusive) lock */
  lock.l_whence = SEEK_SET; /* base for seek offsets */
  lock.l_start = 0;         /* 1st byte in file */
  lock.l_len = 0;           /* 0 here means 'until EOF' */
  lock.l_pid = getpid();    /* process id */
  
  int fd; /* file descriptor to identify a file within a process */
  if ((fd = open(FileName, O_RDONLY)) < 0)  /* -1 signals an error */
    report_and_exit("open to read failed...");

  /* If the file is write-locked, we can't continue. */
  fcntl(fd, F_GETLK, &lock); /* sets lock.l_type to F_UNLCK if no write lock */
  if (lock.l_type != F_UNLCK)
    report_and_exit("file is still write locked...");

  lock.l_type = F_RDLCK; /* prevents any writing during the reading */
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("can't get a read-only lock...");

   
  /* Read the bytes (they happen to be ASCII codes) one at a time. */

     
  int c; /* buffer for read bytes */
  int i = 0;
  while (read(fd, &c, 1) > 0) {   /* 0 signals EOF */
    buffer[i++] = c;
  }
   buffer[i] = '\0';
   
  /* Release the lock explicitly. */
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("explicit unlocking failed...");

  close(fd);
  return i;
}
