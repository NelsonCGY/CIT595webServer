#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backdata.h"

int total = 0, res_num = 0;
course **courses, **res_courses;

void* send_response(void* req){
    char* request = (char*)req;
    char *avg_c, *m_c;
    course** tmp_courses;
    course* m;
    int i, choose=0;
    float avg;
    //pthread_mutex_lock(&c_lock);
    if(res_num == 0){
        free(res_courses);
        res_num = total;
        res_courses = (course**)malloc(sizeof(course*)*666);
        if(!res_courses){perror("Error malloc space"); exit(-1);}
        for(i=0; i<666; i++){res_courses[i] = courses[i];}
    }
    int tmp_num=res_num;
    course* src_courses[tmp_num];
    for(i=0; i<tmp_num; i++){src_courses[i] = res_courses[i];}
    //pthread_mutex_unlock(&c_lock);
    if(request[0] == 'R'){
        tmp_num = total;
        tmp_courses = (course**)malloc(sizeof(course*)*666);
        if(!tmp_courses){perror("Error malloc space"); exit(-1);}
        for(i=0; i<666; i++){tmp_courses[i] = courses[i];}
    }
    else if(request[0] == 'C'){
        char major[5];
        int c_num;
        sscanf(request, "%*s %s %d",major,&c_num);
        tmp_courses = find_course(major, c_num, src_courses, &tmp_num);
    }
    else if(request[0] == 'I'){
        char instructor[30];
        sscanf(request, "%*s %s",instructor);
        tmp_courses = find_instructor(instructor, src_courses, &tmp_num);
    }
    else if(request[0] == 'F'){
        int dir, kind, enroll;
        float three;
        if(request[4] == '1'){
            sscanf(request,"%*s %d %*d %d", &dir, &enroll);
            tmp_courses = filter_enroll(dir, enroll, src_courses, &tmp_num);
        }
        else{
            sscanf(request,"%*s %d %d %f", &dir, &kind, &three);
            tmp_courses = filter_three(dir, kind, three, src_courses, &tmp_num);
        }
    }
    else if(request[0] == 'S'){
        int dir, kind;
        sscanf(request,"%*s %d %d", &dir, &kind);
        tmp_courses = sort_four(dir, kind, src_courses, tmp_num);
    }
    else if(request[0] == 'A'){
        tmp_courses = (course**)malloc(sizeof(course*)*tmp_num);
        if(!tmp_courses){perror("Error malloc space"); exit(-1);}
        for(i=0; i<tmp_num; i++){tmp_courses[i] = src_courses[i];}
        int kind;
        sscanf(request,"%*s %d", &kind);
        avg = avg_four(kind, src_courses, tmp_num);
        avg_c = (char*)malloc(sizeof(char)*50);
        if(kind==1) sprintf(avg_c, "The average of course enroll is %d", (int)avg);
        else if(kind==2) sprintf(avg_c, "The average of course quality is %.2f", avg);
        else if(kind==3) sprintf(avg_c, "The average of course difficulty is %.2f", avg);
        else if(kind==4) sprintf(avg_c, "The average of instructor quality is %.2f", avg);
        choose=1;
    }
    else if(request[0] == 'M'){
        tmp_courses = (course**)malloc(sizeof(course*)*tmp_num);
        if(!tmp_courses){perror("Error malloc space"); exit(-1);}
        for(i=0; i<tmp_num; i++){tmp_courses[i] = src_courses[i];}
        int kind;
        sscanf(request,"%*s %d", &kind);
        m = max_four(kind, src_courses, tmp_num);
        m_c = (char*)malloc(sizeof(char)*180);
        if(kind==1) sprintf(m_c, "The maximum of course enroll is:\n");
        else if(kind==2) sprintf(m_c, "The maximum of course quality is:\n");
        else if(kind==3) sprintf(m_c, "The maximum of course difficulty is:\n");
        else if(kind==4) sprintf(m_c, "The maximum of instructor quality is:\n");
        choose=2;
    }
    else if(request[0] == 'N'){
        tmp_courses = (course**)malloc(sizeof(course*)*tmp_num);
        if(!tmp_courses){perror("Error malloc space"); exit(-1);}
        for(i=0; i<tmp_num; i++){tmp_courses[i] = src_courses[i];}
        int kind;
        sscanf(request,"%*s %d", &kind);
        m = min_four(kind, src_courses, tmp_num);
        m_c = (char*)malloc(sizeof(char)*180);
        if(kind==1) sprintf(m_c, "The minimum of course enroll is:\n");
        else if(kind==2) sprintf(m_c, "The minimum of course quality is:\n");
        else if(kind==3) sprintf(m_c, "The minimum of course difficulty is:\n");
        else if(kind==4) sprintf(m_c, "The minimum of instructor quality is:\n");
        choose=2;
    }

    char** response;
    if(choose==0){
        response = tostring(tmp_courses,tmp_num);
        for(i=0;i<tmp_num;i++){
            printf("%s, %d\n", response[i], i+1);
        }
    }
    else if(choose==1){
        response = (char**)malloc(sizeof(char*)*2);
        response[0] = avg_c;
        response[1] = NULL;
        printf("%s\n", response[0]);
    }
    else{
        response = tostring(&m,1);
        strcat(m_c, response[0]);
        printf("%s\n", m_c);
        free(m_c);
    }

    free_char(response);
    //pthread_mutex_lock(&c_lock);
    free(res_courses);
    res_courses = tmp_courses;
    res_num = tmp_num;
    //pthread_mutex_lock(&c_lock);
    return NULL;
}

int main(){
    courses = readfile("course_evals.txt", &total);
    send_response("R");
    printf("\n\n\nTestTestTest:\n\n");

    send_response("R");
    send_response("C c 5");
    send_response("I Ben");
    send_response("F 1 1 30");
    send_response("F 2 4 1.7");
    send_response("S 1 4");
    send_response("A 1");
    send_response("M 3");
    send_response("N 2");

    free(res_courses);
    free_courses_all(courses);
    return 1;
}
