/*!
*   @file tcpclient.cpp
*
*   This is part of the Kria Kitchen Lifeguard Project
*  TCP socket client, connects to the TCP server and retrieves
*  the last data published by the people detector. 
*
*
*   Written by Enrique Albertos, with contributions from the 
*   open source community and Xilinx libraries.
*   Created: 2022-03-29
*   Public Domain
*/

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
    
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
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
  
   ret = recv(sockfd, buf, sizeof(buf), 0);
   if(ret == -1)
   {
        perror("Socket recv");
   }
   else
   {
        printf ("Message received from server:- %s : size = %d\n", buf, ret); 
   }

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
