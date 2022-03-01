#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memset

#include <sys/types.h>      
#include <sys/socket.h>
#include <netinet/in.h>   //struct sockaddr_in
#include <arpa/inet.h>    //inet_addr()

int main()
{
   int sockfd;
   int ret = 0;
   char buf[256];
   
   struct sockaddr_in server_addr;
    
    //int socket(int domain, int type, int protocol);
    
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/*   
       struct sockaddr_in {
        short int          sin_family;  // Address family, AF_INET
        unsigned short int sin_port;    // Port number
        struct in_addr     sin_addr;    // Internet address
        unsigned char      sin_zero[8]; // Same size as struct sockaddr
    };

    
    // Internet address (a structure for historical reasons)
    struct in_addr {
        uint32_t s_addr; // that's a 32-bit int (4 bytes)
    };
*/
   
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(8000);
   server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     
   ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
   
   if (ret == 0)
   {
        printf ("Connection to server successed \n");
   }
   else
   {
        perror("Client connect");     
   }  
   
   memset(buf, sizeof(buf), 0);
   //ssize_t recv(int sockfd, void *buf, size_t len, int flags);
   ret = recv(sockfd, buf, sizeof(buf), 0);
   if(ret == -1)
   {
        perror("Socket recv");
   }
   else
   {
        printf ("Message received from server:- %s : size = %d\n", buf, ret); 
   }

   //ssize_t send(int sockfd, const void *buf, size_t len, int flags);
   
   memset(buf, sizeof(buf), 0);
   strcpy(buf, "Hello to Socket programming : Message from Client"); 
   
   ret = send(sockfd, buf, sizeof(buf), 0);
   if(ret == -1)
   {
        perror("Socket send");
   }
   else
   {
        printf ("Message sent to server:- %s : size = %d\n", buf, ret); 
   }
 
}
