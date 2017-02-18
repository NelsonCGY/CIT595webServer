#include "backdata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

course** readfile(char* filename, int* total){
    FILE* c_evals = fopen(filename,"rw");
    if(!c_evals){perror("Error opening file"); exit(-1);}
    printf("File opened!\n");
    course** courses = (course**)malloc(sizeof(course*)*700);
    if(!courses){perror("Error malloc space"); exit(-1);}
    int i;
    for(i=0; i<700; i++){courses[i] = NULL;}
    *total = 0;
    char name;
    while(!feof(c_evals)){
        course* newc = (course*)malloc(sizeof(course));
        memset(newc,0,sizeof(course));
        if(!courses){perror("Error malloc space"); exit(-1);}
        i=0; name = fgetc(c_evals);
        while(name!='-'){
            newc->major[i] = name; i++;
            name = fgetc(c_evals);
        }
        fscanf(c_evals,"%d-", &(newc->c_num));
        i=0; name = fgetc(c_evals);
        while(name!=','){
            newc->c_subnum[i] = name; i++;
            name = fgetc(c_evals);
        } fgetc(c_evals);
        i=0; name = fgetc(c_evals);
        while(name!=','){
            newc->instructor[i] = name; i++;
            name = fgetc(c_evals);
        } fgetc(c_evals);
        fscanf(c_evals,"%d, %f, %f, %f\n", &(newc->enroll), &(newc->c_quality), &(newc->c_difficulty), &(newc->i_quality));
        courses[*total] = newc;
        (*total)++;
    }
    fclose(c_evals);
    return courses;
}


/*
course** find_course(char* major, int c_num, int* num);
course** find_instructor(char* instructor, int* num);

course** filter_enroll(int k, course** cources, int* num);
course** filter_three(int kind, float k, course** cources, int* num);

void sort_enroll(int dir, course** cources, int num);
void sort_three(int dir, int kind, course** cources, int num);

int avg_enroll(course** sourses, int num);
float avg_three(int kind, course** courses, int num);*/

course* max_four(int kind, course** courses, int num){
    course* res;
    int i, max=0;
    float maxf=0.0;
    for(i=0; i<num; i++){
        if(kind==1){
            if(courses[i]->enroll >= max){
                max = courses[i]->enroll;
                res = courses[i];
            }
        }
        else if(kind==2){
            if(courses[i]->c_quality >= maxf){
                maxf = courses[i]->c_quality;
                res = courses[i];
            }
        }
        else if(kind==3){
            if(courses[i]->c_difficulty >= maxf){
                maxf = courses[i]->c_difficulty;
                res = courses[i];
            }
        }
        else if(kind==4){
            if(courses[i]->i_quality >= maxf){
                maxf = courses[i]->i_quality;
                res = courses[i];
            }
        }
    }
    return res;
}

course* min_four(int kind, course** courses, int num){
    course* res;
    int i, min=1000;
    float minf=1000.0;
    for(i=0; i<num; i++){
        if(kind==1){
            if(courses[i]->enroll <= min){
                min = courses[i]->enroll;
                res = courses[i];
            }
        }
        else if(kind==2){
            if(courses[i]->c_quality <= minf){
                minf = courses[i]->c_quality;
                res = courses[i];
            }
        }
        else if(kind==3){
            if(courses[i]->c_difficulty <= minf){
                minf = courses[i]->c_difficulty;
                res = courses[i];
            }
        }
        else if(kind==4){
            if(courses[i]->i_quality <= minf){
                minf = courses[i]->i_quality;
                res = courses[i];
            }
        }
    }
    return res;
}

char** tostring(course** courses, int num){
    char** res = (char**)malloc(sizeof(char*)*(num+1));
    if(!res){perror("Error malloc space"); exit(-1);}
    int i;
    for(i=0; i<num+1; i++){res[i] = NULL;}
    for(i=0; i<num; i++){
        char* c_info = (char*)malloc(sizeof(char)*65);
        memset(c_info,0,65);
        if(!c_info){perror("Error malloc space"); exit(-1);}
        sprintf(c_info, "%s-%d-%s, %s, %d, %.2f, %.2f, %.2f", (courses[i])->major, (courses[i])->c_num, (courses[i])->c_subnum, (courses[i])->instructor, (courses[i])->enroll, (courses[i])->c_quality,(courses[i])->c_difficulty, (courses[i])->i_quality);
        res[i] = c_info;
    }
    return res;
}

void free_char(char** res){
     int i;
     for(i=0; res[i]; i++){
        free(res[i]);
     }
     free(res);
}

void free_courses_all(course** courses){
    int i;
    for(i=0; courses[i]; i++){
        free(courses[i]);
    }
    free(courses);
}

int main(){
    int total, i;
    course** courses = readfile("course_evals.txt", &total);
    printf("total %d\n", total);
    char** res = tostring(courses,total);
    for(i=0;i<total;i++){
        //printf("%s-%d-%s, %s, %d, %.2f, %.2f, %.2f, %d\n", (courses[i])->major, (courses[i])->c_num, (courses[i])->c_subnum, (courses[i])->instructor, (courses[i])->enroll, (courses[i])->c_quality,(courses[i])->c_difficulty, (courses[i])->i_quality, i);
        printf("%s, %d\n", res[i], i);
    }
    course* minR = min_four(1, courses, total);
    printf("\nMinR: %s-%d-%s, %s, %d, %.2f, %.2f, %.2f\n", minR->major, minR->c_num, minR->c_subnum, minR->instructor, minR->enroll, minR->c_quality, minR->c_difficulty, minR->i_quality);
    course* maxD = max_four(3, courses, total);
    printf("\nMaxD: %s-%d-%s, %s, %d, %.2f, %.2f, %.2f\n", maxD->major, maxD->c_num, maxD->c_subnum, maxD->instructor, maxD->enroll, maxD->c_quality, maxD->c_difficulty, maxD->i_quality);
    free_char(res);
    free_courses_all(courses);
    return 0;
}
