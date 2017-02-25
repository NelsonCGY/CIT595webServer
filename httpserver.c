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

pthread_mutex_t t_lock, c_lock;
int[] thread_check  = int[10];

typedef struct Thread_input{
  int index;
  char* req[30];
} thread_input;


void* sent_response(void* p){
  thread_input* input = (thread_input) p;
  int i;
  if(strcmp(input, "test.html")){
    int total;
    course** courses = readfile(file_name, &total);
    char** res = tostring(courses,total);
    char* reply = malloc(sizeof(char) * 100 * total);
    strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html>");
    for(i=0;i<total;i++){
        char* newest = res[i];
        strcat(reply, "<p>");
        strcat(reply, newest);
    }
  }else if(strcmp(request, "/sort.html")){

  }else if(strcmp(request, "/filter.html")){

  }
  char* reply_after = "</html>\n";
  strcat(reply, reply_after);
  send(fd, reply, strlen(reply), 0);
  free(reply);
  pthread_mutex_lock(&lock);
  thread_check[input->index] = 0;
  pthread_mutex_unlock(&lock);


}



int start_server(int PORT_NUMBER, char* file_name)
{
      pthread_t* thread_array = pthread_t[10];
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
      course** courses = readfile(file_name, &total);
      char** res = tostring(courses,total);
      // char* newest = res[0];
      // printf("%s", newest);
      // 4. accept: wait here until we get a connection on that port
      int sin_size = sizeof(struct sockaddr_in);
      int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
      if (fd != -1) {
      	printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            
      	// buffer to read data into
      	char request[1024];
      	//loop start here-Haoran
        while(1){
        	// 5. recv: read incoming message (request) into buffer
        	int bytes_received = recv(fd,request,1024,0);
        	// null-terminate the string
        	request[bytes_received] = '\0';
        	// print it to standard out
        	printf("This is the incoming request:\n%s\n", request);

          char rq[30];
          int i = 5;
          while(!(request[i] == ' ' && request[i+1] == 'H' && request[i+2] == 'T' && request[i+3] == 'T' && request[i+4] == 'P' && request[i+5] == '/')){
            rq[i-5] =  request[i];
            i++;
          }
          int r;
          int r_join;
          void* thread_return;
          while(1){
            for(thread_index = 0; thread_index < 10; thread_index++){
              if(thread_array[thread_index] == 0){
                r_join = pthread_join(thread_array[thread_index], &thread_return);
                thread_input new_input;
                new_input.index = thread_index;
                strcpy(new_input.req, rq);
                r = pthread_create(&thread_array[thread_index], NULL, &fun1, &new_input);
                if(r != 0){
                  printf("THREAD CREATION ERROR");
                }
                thread_check[thread_index] = 1;
                break;
              }
            }
            break;
          }

        	// this is the message that we'll send back
          // use for loop to display all the strings in the file-haoran
          // char* reply = malloc(sizeof(char) * 100 * total);
          // strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html>");
          // for(i=0;i<total;i++){
          //   char* newest = res[i];
          //   strcat(reply, "<p>");
          //   strcat(reply, newest);
          // }
            // printf("%s", reply);
            // 6. send: send the outgoing message (response) over the socket
            // note that the second argument is a char*, and the third is the number of chars 
          
        }
      	
      	// 7. close: close the connection
      	close(fd);
      	printf("Server closed connection\n");
      }

  // 8. close: close the socket
  close(sock);
  printf("Server shutting down\n");

  return 0;
} 



int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 3) {
      printf("\nUsage: %s [port_number]\n", argv[0]);
      exit(-1);
  }

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }
  char* file_name = argv[2];
  printf("File name is %s", file_name);
  // course** courses = readfile(file_name, &total);
  // char** res = tostring(courses,total);
  // for(i=0;i<total;i++){
  //     //printf("%s-%d-%s, %s, %d, %.2f, %.2f, %.2f, %d\n", (courses[i])->major, (courses[i])->c_num, (courses[i])->c_subnum, (courses[i])->instructor, (courses[i])->enroll, (courses[i])->c_quality,(courses[i])->c_difficulty, (courses[i])->i_quality, i);
  //     printf("%s, %d\n", res[i], i);
  // }
  start_server(port_number, file_name);
}

