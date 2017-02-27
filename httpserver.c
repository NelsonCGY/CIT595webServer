/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
// #include "backdata.h"
#include "backdata.c"

int start_server(int PORT_NUMBER, char* file_name)
{

      // structs to represent the server and client
      struct sockaddr_in server_addr,client_addr;    
      
      int sock; // socket descriptor

      // 1. socket: creates a socket descriptor that you later use to make other system calls
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      	perror("Socket");
      	exit(1);
      }
      int temp;
      if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
        perror("Setsockopt");
        exit(1);
      }

      // configure the server
      server_addr.sin_port = htons(PORT_NUMBER); // specify port number
      server_addr.sin_family = AF_INET;         
      server_addr.sin_addr.s_addr = INADDR_ANY; 
      bzero(&(server_addr.sin_zero),8); 
      
      // 2. bind: use the socket and associate it with the port number
      if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Unable to bind");
        exit(1);
      }

      // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
      if (listen(sock, 1) == -1) {
        perror("Listen");
        exit(1);
      }
          
      // once you get here, the server is set up and about to start listening
      printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
      fflush(stdout);

      //Read file here - haoran
      int total, i;
      //res get the data
      // char* newest = res[0];
      // printf("%s", newest);
      // 4. accept: wait here until we get a connection on that port
      int sin_size = sizeof(struct sockaddr_in);
      int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
      if (fd != -1) {
      	printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        course** courses = readfile(file_name, &total);
        char** res = tostring(courses,total);
      	// buffer to read data into
      	char request[1024];
        char* reply = malloc(sizeof(char) * 100000);
        char* cur_line = malloc(sizeof(char) * 100);
        while(1){
        	// 5. recv: read incoming message (request) into buffer
        	int bytes_received = recv(fd,request,1024,0);
        	// null-terminate the string
        	request[bytes_received] = '\0';
        	// print it to standard out
        	printf("This is the incoming request:\n%s\n", request);
          /**
            Incorporate the html file which is in the same folder
            Yiwei Guo
          */
          if (strcmp(request, "GET /sort/1 HTTP/1.1")){
              courses = sort_enroll(1, courses, total);
          }
          FILE* header = fopen("template.html", "r");
          /*Generate the table content dynamically*/
          char* table = print_table(courses, total);
          /*Process the course info*/
          while(!feof(header))
          {     
              /*
              Read from html file line by line
              */
              char* line = fgets(cur_line, 100, header);
              if (strcmp(line, "<!--add dynamic coutent-->\n") == 0){
                  strcat(reply, cur_line);
                  strcat(reply,table);
              }else{
                  strcat(reply, cur_line);
              }

          }
          send(fd, reply, strlen(reply), 0);
          memset(reply, 0,sizeof(char) * 10000);
          memset(request,0,sizeof(char)*1024);
        } 
  }
  // 7. close: close the connection
  close(fd);
  printf("Server closed connection\n");

  // 8. close: close the socket
  close(sock);
  printf("Server shutting down\n");

  return 0;
} 



int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 3) {
      printf("\nUsage: %s port_number file_name\n", argv[0]);
      exit(-1);
  }

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }
  char* file_name = argv[2];
  printf("File name is %s", file_name);
  start_server(port_number, file_name);
}

