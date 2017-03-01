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
#include <pthread.h>
#include <time.h>
#include "backdata.h"

pthread_mutex_t t_lock, c_lock;
int thread_check[10] = {0};
int total = 0, res_num = 0, initial = 0;
course **courses, **res_courses;

typedef struct Thread_input
{
    int index;
    char req[30];
    int fd;
} thread_input;


void* send_response(void* p)
{
    thread_input* input = (thread_input*) p;
    char* request = input->req;
    printf("Get a request: %s\n", request);
    char avg_c[50] = {'\0'};
    course** tmp_courses;
    course* m;
    int i, choose=0;
    float avg;
    pthread_mutex_lock(&c_lock);
    if(res_num == 0)
    {
        free(res_courses);
        res_num = total;
        res_courses = (course**)malloc(sizeof(course*)*666);
        if(!res_courses)
        {
            perror("Error malloc space:");
            exit(-1);
        }
        for(i=0; i<666; i++)
        {
            res_courses[i] = NULL;
            res_courses[i] = courses[i];
        }
    }
    int tmp_num=res_num;
    course* src_courses[tmp_num];
    for(i=0; i<tmp_num; i++)
    {
        src_courses[i] = res_courses[i];
    }
    pthread_mutex_unlock(&c_lock);
    if(request[0] == 'R')
    {
        tmp_num = total;
        tmp_courses = (course**)malloc(sizeof(course*)*666);
        if(!tmp_courses)
        {
            perror("Error malloc space:");
            exit(-1);
        }
        for(i=0; i<666; i++)
        {
            tmp_courses[i] = NULL;
            tmp_courses[i] = courses[i];
        }
    }
    else if(request[0] == 'C')
    {
        char major[5] = {'\0'};
        int c_num = 0;
        for(i=2; request[i] != '_'; i++)
        {
            major[i - 2] = request[i];
        }
        for(++i; i<strlen(request); i++)
        {
            c_num = c_num*10 + (request[i] - '0');
        }
        tmp_courses = find_course(major, c_num, src_courses, &tmp_num);
    }
    else if(request[0] == 'I')
    {
        char instructor[30] = {'\0'};
        for(i=2; request[i] != '_' && i < strlen(request); i++)
        {
            instructor[i - 2] = request[i];
        }
        if(i<strlen(request))
        {
            instructor[i - 2] = ' ';
        }
        for(++i; i<strlen(request); i++)
        {
            instructor[i - 2] = request[i];
        }
        tmp_courses = find_instructor(instructor, src_courses, &tmp_num);
    }
    else if(request[0] == 'F')
    {
        int dir, kind, enroll;
        float three;
        if(request[4] == '1')
        {
            sscanf(request,"%*1s_%d_%*d_%d", &dir, &enroll);
            tmp_courses = filter_enroll(dir, enroll, src_courses, &tmp_num);
        }
        else
        {
            sscanf(request,"%*1s_%d_%d_%f", &dir, &kind, &three);
            tmp_courses = filter_three(dir, kind, three, src_courses, &tmp_num);
        }
    }
    else if(request[0] == 'S')
    {
        int dir, kind;
        sscanf(request,"%*1s_%d_%d", &dir, &kind);
        tmp_courses = sort_four(dir, kind, src_courses, tmp_num);
    }
    else if(request[0] == 'A')
    {
        tmp_courses = (course**)malloc(sizeof(course*)*tmp_num);
        if(!tmp_courses)
        {
            perror("Error malloc space:");
            exit(-1);
        }
        for(i=0; i<tmp_num; i++)
        {
            tmp_courses[i] = NULL;
            tmp_courses[i] = src_courses[i];
        }
        int kind;
        sscanf(request,"%*1s_%d", &kind);
        avg = avg_four(kind, src_courses, tmp_num);
        if(kind==1) sprintf(avg_c, "The average of course enroll is %d", (int)avg);
        else if(kind==2) sprintf(avg_c, "The average of course quality is %.2f", avg);
        else if(kind==3) sprintf(avg_c, "The average of course difficulty is %.2f", avg);
        else if(kind==4) sprintf(avg_c, "The average of instructor quality is %.2f", avg);
        choose=1;
    }
    else if(request[0] == 'M')
    {
        tmp_courses = (course**)malloc(sizeof(course*)*tmp_num);
        if(!tmp_courses)
        {
            perror("Error malloc space:");
            exit(-1);
        }
        for(i=0; i<tmp_num; i++)
        {
            tmp_courses[i] = NULL;
            tmp_courses[i] = src_courses[i];
        }
        int kind;
        sscanf(request,"%*1s_%d", &kind);
        m = max_four(kind, src_courses, tmp_num);
        choose = 2;
    }
    else if(request[0] == 'N')
    {
        tmp_courses = (course**)malloc(sizeof(course*)*tmp_num);
        if(!tmp_courses)
        {
            perror("Error malloc space:");
            exit(-1);
        }
        for(i=0; i<tmp_num; i++)
        {
            tmp_courses[i] = NULL;
            tmp_courses[i] = src_courses[i];
        }
        int kind;
        sscanf(request,"%*1s_%d", &kind);
        m = min_four(kind, src_courses, tmp_num);
        choose = 2;
    }

    // this is the message that we'll send back
    // use for loop to display all the strings in the file
    // incorporate the html file which is in the same folder
    FILE* header;
    char *reply, *table_html, *line;
    char cur_line[100] = {'\0'};
    if(choose==0)
    {
        reply = (char*)malloc(sizeof(char) * 300 * (tmp_num + 160));
        memset(reply, 0, 300 * (tmp_num + 160));
        header = fopen("template.html", "r");
        /*Generate the table content dynamically*/
        table_html = print_table(tmp_courses, tmp_num);
        /*Process the course info*/
        while(!feof(header))
        {
            /* Read from html file line by line */
            line = fgets(cur_line, 100, header);
            if (strncmp(line, "<!--add dynamic coutent-->", 26) == 0)
            {
                strcat(reply, cur_line);
                strcat(reply,table_html);
            }
            else
            {
                strcat(reply, cur_line);
            }
        }
    }
    else if(choose==1)
    {
        reply = (char*)malloc(sizeof(char) * 300 * 160);
        memset(reply, 0, 300 * 160);
        header = fopen("template.html", "r");
        table_html = (char*)malloc((sizeof(char) * 50));
        memset(table_html, 0, 50);
        strcpy(table_html, avg_c);
        while(!feof(header))
        {
            /* Read from html file line by line */
            line = fgets(cur_line, 100, header);
            if (strncmp(line, "<!--add dynamic coutent-->", 26) == 0)
            {
                strcat(reply, cur_line);
                strcat(reply, "<tr>\n<td>");
                strcat(reply,table_html);
                strcat(reply, "</td>\n</tr>\n");
            }
            else
            {
                strcat(reply, cur_line);
            }
        }
    }
    else
    {
        reply = (char*)malloc(sizeof(char) * 300 * 160);
        memset(reply, 0, 300 * 160);
        header = fopen("template.html", "r");
        table_html = print_table(&m, 1);
        while(!feof(header))
        {
            /* Read from html file line by line */
            line = fgets(cur_line, 100, header);
            if (strncmp(line, "<!--add dynamic coutent-->", 26) == 0)
            {
                strcat(reply, cur_line);
                strcat(reply,table_html);
            }
            else
            {
                strcat(reply, cur_line);
            }
        }
    }

    // 6. send: send the outgoing message (response) over the socket
    // note that the second argument is a char*, and the third is the number of chars
    send(input->fd, reply, strlen(reply), 0);
    close(input->fd);
    printf("Sent! Server closed connection\n");
    fflush(stdout);

    fclose(header);
    free(reply);
    free(table_html);
    pthread_mutex_lock(&c_lock);
    free(res_courses);
    res_courses = tmp_courses;
    res_num = tmp_num;
    pthread_mutex_unlock(&c_lock);

    printf("Start wait!\n");
    sleep(5);
    printf("Wait ended!\n");

    pthread_mutex_lock(&t_lock);
    thread_check[input->index] = 0;
    pthread_mutex_unlock(&t_lock);

    printf("Exit thread #%d!\n", input->index);
    return NULL;
}

void* try_exit(void* p)
{
    char input = 'a';
    while(input != 'q' && input != 'Q')
    {
        input = getchar();
    }
    initial = 0;
    printf("Request to shut down server!\n");
    if(courses)
    {
        printf("\n******\nPlease request one more time to shut down server\n******\n\n");
    }
    return NULL;
}


int start_server(int PORT_NUMBER, char* file_name)
{

    // structs to represent the server and client
    struct sockaddr_in server_addr,client_addr;

    int sock; // socket descriptor

    // 1. socket: creates a socket descriptor that you later use to make other system calls
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        printf("\n******\nPlease type q in the console to shut down server\n******\n\n");
        return -1;
    }
    int temp = 0;
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1)
    {
        perror("Setsockopt");
        printf("\n******\nPlease type q in the console to shut down server\n******\n\n");
        return -1;
    }

    // configure the server
    server_addr.sin_port = htons(PORT_NUMBER); // specify port number
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero),8);

    // 2. bind: use the socket and associate it with the port number
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Unable to bind");
        printf("\n******\nPlease type q in the console to shut down server\n******\n\n");
        return -1;
    }

    // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
    if (listen(sock, 1) == -1)
    {
        perror("Listen");
        printf("\n******\nPlease type q in the console to shut down server\n******\n\n");
        return -1;
    }

    // once you get here, the server is set up and about to start listening
    printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
    fflush(stdout);

    // 4. accept: wait here until we get a connection on that port
    int sin_size = sizeof(struct sockaddr_in);
    int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
    if (fd != -1)
    {
        printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        // buffer to read data into
        char request[1024] = {'\0'};

        // 5. recv: read incoming message (request) into buffer
        int bytes_received = recv(fd,request,1024,0);
        // null-terminate the string
        request[bytes_received] = '\0';
        // print it to standard out
        printf("This is the incoming request:\n%s\n", request);

        // initial output
        pthread_t thread_array[10];
        thread_input new_input[10];
        int has_thread[10]= {0};
        courses = readfile(file_name, &total);
        pthread_mutex_init(&c_lock,NULL);
        pthread_mutex_init(&t_lock,NULL);
        int i, thread_index, created;

        if(initial==0)
        {
            thread_index = 0;
            new_input[thread_index].index = thread_index;
            int j;
            for(j=0; j<30; j++)
            {
                new_input[thread_index].req[j] = '\0';
            }
            new_input[thread_index].req[0] = 'R';
            new_input[thread_index].fd = fd;
            int r;
            r = pthread_create(&(thread_array[thread_index]), NULL, &send_response, &(new_input[thread_index]));
            if(r != 0)
            {
                printf("THREAD CREATION ERROR");
            }
            thread_check[thread_index] = 1;
            has_thread[thread_index] = 1;
            printf("Initialized!\nAssigned to thread #%d\n", thread_index);
        }
        initial=1;

        // operation loop start here
        while(initial==1)
        {
            printf("Accepting next request:\n");

            fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
            bytes_received = recv(fd,request,1024,0);
            request[bytes_received] = '\0';
            printf("This is the incoming request:\n%s\n", request);
            if(strncmp(request, "GET /favicon.ico", 16)==0)
            {
                continue;
            }

            char rq[30] = {'\0'};
            i = 5;
            if(request[i] == 'Q')
            {
                printf("Request to exit!\n");
                break; // exit program
            }
            while(!(request[i] == ' ' && request[i+1] == 'H' && request[i+2] == 'T' && request[i+3] == 'T' && request[i+4] == 'P' && request[i+5] == '/'))
            {
                rq[i-5] =  request[i];
                i++;
            }
            created = 0;


            while(initial==1)
            {
                for(thread_index = 0; thread_index < 10; thread_index++)
                {
                    if(thread_check[thread_index] == 0)
                    {
                        if(has_thread[thread_index] == 1)
                        {
                            pthread_join(thread_array[thread_index], NULL);
                            has_thread[thread_index] = 0;
                        }
                        new_input[thread_index].index = thread_index;
                        int j;
                        for(j=0; j<30; j++)
                        {
                            new_input[thread_index].req[j] = '\0';
                        }
                        strcpy(new_input[thread_index].req, rq);
                        new_input[thread_index].fd = fd;
                        int r;
                        r = pthread_create(&(thread_array[thread_index]), NULL, &send_response, &(new_input[thread_index]));
                        if(r != 0)
                        {
                            printf("THREAD CREATION ERROR");
                        }
                        thread_check[thread_index] = 1;
                        has_thread[thread_index] = 1;
                        printf("Assigned to thread #%d\n", thread_index);
                        created = 1;
                        break;
                    }
                }
                if(created)
                {
                    break;
                }
            }
        }

        for(thread_index = 0; thread_index < 10; thread_index++)
        {
            if(has_thread[thread_index] == 1)
            {
                pthread_join(thread_array[thread_index], NULL);
            }
        }
        if(res_courses)
        {
            free(res_courses);
        }
        free_courses_all(courses);
        courses = NULL;
        printf("Response ended!\n");
        // operation loop ends here

        // 7. close: close the connection
        close(fd);
        printf("Server closed connection\n");
    }

    // 8. close: close the socket
    close(sock);
    

    return 0;
}



int main(int argc, char *argv[])
{
    // check the number of arguments
    if (argc != 3)
    {
        printf("\nUsage: %s [port_number]\n", argv[0]);
        exit(-1);
    }

    int port_number = atoi(argv[1]);
    if (port_number <= 1024)
    {
        printf("\nPlease specify a port number greater than 1024\n");
        exit(-1);
    }
    char* file_name = argv[2];

    pthread_t exitT;
    pthread_create(&exitT, NULL, &try_exit, NULL);

    start_server(port_number, file_name);

    if(initial == 1)
    {
        printf("\n******\nPlease type q in the console to shut down server\n******\n\n");
    }
    pthread_join(exitT, NULL);
    printf("Server shutting down\n");
    sleep(30);
    printf("Server shut down\n");
}

