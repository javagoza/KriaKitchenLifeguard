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
        memset(buf, 0, sizeof(sendBuff));
        int readLenght = readData(sendBuff, sizeof(sendBuff));
        // snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
        sleep(1);
     }
}

int main2 (int argc, char *argv[]) {
   int sockfd, sockfd_client;
   int ret = 0;
   socklen_t sock_pkt_size = 0;
   char buf[BUF_SIZE];
   
   struct sockaddr_in server_addr, client_addr;
    
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd != -1) {
        printf ("Socket creation success : sockfd = %d \n", sockfd);
   } else {
        perror("socket sockfd");        
        return -1;
   }  
   
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(8000);
   server_addr.sin_addr.s_addr = inet_addr("192.168.2.95");     
   ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
   if (ret == 0)  {
        printf (" Bind Success \n");
   } else  {
        perror("Bind");        
        return -1;
   }    
   while(1) {
      ret = listen(sockfd, 5);
      if (ret == 0) {
           printf (" Listen Success \n");
      } else {
           perror("listen");        
           return -1;
      }    

      sock_pkt_size = sizeof(struct sockaddr_in);

      sockfd_client = accept(sockfd, (struct sockaddr *)&client_addr, &sock_pkt_size);
      if (sockfd_client != -1) {
           printf ("Socket creation success : sockfd_client = %d \n", sockfd_client);
      } else {
           perror("socket sockfd_client");        
           return -1;
      }
      
      memset(buf, 0, sizeof(buf));
      int readLenght = readData(buf, sizeof(buf));
      ret = send(sockfd_client, buf, readLenght, 0);
      if(ret == -1) {
           perror("Socket send");
      } else {
           printf ("Message sent to client:- %s : size = %d\n", buf, ret); 
      }
      memset(buf, 0, sizeof(buf));
      ret = recv(sockfd_client, buf, sizeof(buf), 0);
      if(ret == -1) {
           perror("Socket recv");
      } else {
           printf ("Message received from client :- %s : size = %d\n", buf, ret); 
      }   
   }   
 }


void report_and_exit(const char* msg) {
  perror(msg);
  //exit(-1); /* EXIT_FAILURE */
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
    //write(STDOUT_FILENO, &c, 1); /* write one byte to the standard output */
    buffer[i++] = c;
  }
   buffer[i] = '\0';
   
  //int ret_in = read (fd, &buffer, bufferSize);  

  /* Release the lock explicitly. */
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("explicit unlocking failed...");

  close(fd);
  return i;
}
