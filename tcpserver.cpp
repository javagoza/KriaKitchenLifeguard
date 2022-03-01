#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memset

#include <sys/types.h>      
#include <sys/socket.h>
#include <netinet/in.h>   //struct sockaddr_in
#include <arpa/inet.h>    //inet_addr()
#include <unistd.h>       //sleep(1)

int main()
{
   int sockfd, sockfd_client;
   int ret = 0;
   socklen_t sock_pkt_size = 0;
   char buf[] = "Hello to Socket programming : Message from Server";
   
   struct sockaddr_in server_addr, client_addr;
    
    //int socket(int domain, int type, int protocol);
    
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd != -1)
   {
        printf ("Socket creation success : sockfd = %d \n", sockfd);
   }
   else
   {
        perror("socket sockfd");
        
        return -1;
   }
   
   //int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

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
   //server_addr.sin_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
   server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     
   ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
   if (ret == 0)
   {
        printf (" Bind Success \n");
   }
   else
   {
        perror("Bind");
        
        return -1;
   }    
   
   //int listen(int sockfd, int backlog);
   ret = listen(sockfd, 5);
   if (ret == 0)
   {
        printf (" Listen Success \n");
   }
   else
   {
        perror("listen");
        
        return -1;
   }    

   sock_pkt_size = sizeof(struct sockaddr_in);
   //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
   sockfd_client = accept(sockfd, (struct sockaddr *)&client_addr, &sock_pkt_size);
   if (sockfd_client != -1)
   {
        printf ("Socket creation success : sockfd_client = %d \n", sockfd_client);
   }
   else
   {
        perror("socket sockfd_client");
        
        return -1;
   }
   
   //ssize_t send(int sockfd, const void *buf, size_t len, int flags);
   
   ret = send(sockfd_client, buf, sizeof(buf), 0);
   if(ret == -1)
   {
        perror("Socket send");
   }
   else
   {
        printf ("Message sent to client:- %s : size = %d\n", buf, ret); 
   }
   
   memset(buf, sizeof(buf), 0);
   
   //ssize_t recv(int sockfd, void *buf, size_t len, int flags);
   ret = recv(sockfd_client, buf, sizeof(buf), 0);
   if(ret == -1)
   {
        perror("Socket recv");
   }
   else
   {
        printf ("Message received from client :- %s : size = %d\n", buf, ret); 
   }
   
   //while(1);
   sleep(10);
   
 }
