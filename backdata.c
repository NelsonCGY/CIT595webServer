#include "backdata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

course** readfile(char* filename, int* total)  // filename passed by argv[1], total will be counted after courses list is made and will not change, what will change is num in following functions
{
    FILE* c_evals = fopen(filename,"r");
    if(!c_evals)
    {
        perror("Error opening file");
        exit(-1);
    }
    printf("File opened!\n");
    course** courses = (course**)malloc(sizeof(course*)*666);
    if(!courses)
    {
        perror("Error malloc space");
        exit(-1);
    }
    int i;
    for(i=0; i<666; i++)
    {
        courses[i] = NULL;
    }
    *total = 0;
    char name;
    while(!feof(c_evals))
    {
        course* newc = (course*)malloc(sizeof(course));
        memset(newc,0,sizeof(course));
        if(!courses)
        {
            perror("Error malloc space");
            exit(-1);
        }
        i=0;
        name = fgetc(c_evals);
        while(name!='-')
        {
            newc->major[i] = name;
            i++;
            name = fgetc(c_evals);
        }
        fscanf(c_evals,"%d-", &(newc->c_num));
        i=0;
        name = fgetc(c_evals);
        while(name!=',')
        {
            newc->c_subnum[i] = name;
            i++;
            name = fgetc(c_evals);
        }
        fgetc(c_evals);
        i=0;
        name = fgetc(c_evals);
        while(name!=',')
        {
            newc->instructor[i] = name;
            i++;
            name = fgetc(c_evals);
        }
        fgetc(c_evals);
        fscanf(c_evals,"%d, %f, %f, %f\n", &(newc->enroll), &(newc->c_quality), &(newc->c_difficulty), &(newc->i_quality));
        courses[*total] = newc;
        (*total)++;
    }
    fclose(c_evals);
    return courses;
}

char *strupr(char *str)   // private function for case insensitive
{
    char *ptr = str;
    while (*ptr != '\0')
    {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
        ptr++;
    }
    return str;
}

course** find_course(char* major, int c_num, course** courses, int* num)  // "*" for major to find all majors, 0 for c_num to find all course numbers, can also be part of them from the start
{
    course** found = (course**)malloc(sizeof(course*)*((*num)+1));
    if(!found)
    {
        perror("Error malloc space");
        exit(-1);
    }
    int i, tot = *num, range=0;
    for(i=0; i<((*num)+1); i++)
    {
        found[i] = NULL;
    }
    if((strlen(major)!=0 && major[0]!='*') && c_num==0)
    {
        range = 1;
        strupr(major);
    }
    else if((strlen(major)==0 || major[0]=='*') && c_num!=0) range = 2;
    else if((strlen(major)!=0 && major[0]!='*') && c_num!=0)
    {
        range = 3;
        strupr(major);
    }
    *num = 0;
    for(i=0; i<tot; i++)
    {
        if(range==1)
        {
            if(strncmp(major, courses[i]->major, strlen(major)) == 0)
            {
                found[*num] = courses[i];
                (*num)++;
            }
        }
        else if(range==2)
        {
            if((c_num==(courses[i]->c_num/100)) || (c_num==(courses[i]->c_num/10)) || (c_num==(courses[i]->c_num)))
            {
                found[*num] = courses[i];
                (*num)++;
            }
        }
        else if(range==3)
        {
            if(((c_num==(courses[i]->c_num/100)) || (c_num==(courses[i]->c_num/10)) || (c_num==(courses[i]->c_num))) && strncmp(major, courses[i]->major, strlen(major))==0)
            {
                found[*num] = courses[i];
                (*num)++;
            }
        }

    }
    return found;
}

course** find_instructor(char* instructor, course** courses, int* num)  // can be part of the instructor's name from the start
{
    course** found = (course**)malloc(sizeof(course*)*((*num)+1));
    if(!found)
    {
        perror("Error malloc space");
        exit(-1);
    }
    int i, tot = *num;
    for(i=0; i<((*num)+1); i++)
    {
        found[i] = NULL;
    }
    *num = 0;
    strupr(instructor);
    for(i=0; i<tot; i++)
    {
        if(strncmp(instructor, courses[i]->instructor, strlen(instructor)) == 0)
        {
            found[*num] = courses[i];
            (*num)++;
        }
    }
    return found;
}

course** filter_enroll(int dir, int k, course** courses, int* num)  // dir: 1 for larger, 2 for smaller, k for standard
{
    course** filtered = (course**)malloc(sizeof(course*)*((*num)+1));
    if(!filtered)
    {
        perror("Error malloc space");
        exit(-1);
    }
    int i, tot = *num;
    for(i=0; i<((*num)+1); i++)
    {
        filtered[i] = NULL;
    }
    *num = 0;
    for(i=0; i<tot; i++)
    {
        if(dir==1)
        {
            if(courses[i]->enroll >= k)
            {
                filtered[*num] = courses[i];
                (*num)++;
            }
        }
        else if(dir==2)
        {
            if(courses[i]->enroll <= k)
            {
                filtered[*num] = courses[i];
                (*num)++;
            }
        }
    }
    return filtered;
}

course** filter_three(int dir, int kind, float k, course** courses, int* num)  // dir: 1 for larger, 2 for smaller, k for standard, kind: 2 for c_quality, 3 for c_difficulty, 4 for i_quality
{
    course** filtered = (course**)malloc(sizeof(course*)*((*num)+1));
    if(!filtered)
    {
        perror("Error malloc space");
        exit(-1);
    }
    int i, tot = *num;
    for(i=0; i<((*num)+1); i++)
    {
        filtered[i] = NULL;
    }
    *num = 0;
    for(i=0; i<tot; i++)
    {
        if(kind==2)
        {
            if(dir==1)
            {
                if(courses[i]->c_quality >= k-0.0001)
                {
                    filtered[*num] = courses[i];
                    (*num)++;
                }
            }
            else if(dir==2)
            {
                if(courses[i]->c_quality <= k+0.0001)
                {
                    filtered[*num] = courses[i];
                    (*num)++;
                }
            }
        }
        else if(kind==3)
        {
            if(dir==1)
            {
                if(courses[i]->c_difficulty >= k-0.0001)
                {
                    filtered[*num] = courses[i];
                    (*num)++;
                }
            }
            else if(dir==2)
            {
                if(courses[i]->c_difficulty <= k+0.0001)
                {
                    filtered[*num] = courses[i];
                    (*num)++;
                }
            }
        }
        else if(kind==4)
        {
            if(dir==1)
            {
                if(courses[i]->i_quality >= k-0.0001)
                {
                    filtered[*num] = courses[i];
                    (*num)++;
                }
            }
            else if(dir==2)
            {
                if(courses[i]->i_quality <= k+0.0001)
                {
                    filtered[*num] = courses[i];
                    (*num)++;
                }
            }
        }
    }
    return filtered;
}

int cmpERA(const void* a, const void* b)
{
    course** aa = (course**)a;    // private compare functions for qsort
    course** bb = (course**)b;
    return (*aa)->enroll - (*bb)->enroll;
}
int cmpERD(const void* a, const void* b)
{
    course** aa = (course**)a;
    course** bb = (course**)b;
    return (*bb)->enroll - (*aa)->enroll;
}
int cmpCQA(const void* a, const void* b)
{
    course** aa = (course**)a;
    course** bb = (course**)b;
    return (*aa)->c_quality > (*bb)->c_quality ? 1:-1;
}
int cmpCQD(const void* a, const void* b)
{
    course** aa = (course**)a;
    course** bb = (course**)b;
    return (*bb)->c_quality > (*aa)->c_quality ? 1:-1;
}
int cmpCDA(const void* a, const void* b)
{
    course** aa = (course**)a;
    course** bb = (course**)b;
    return (*aa)->c_difficulty > (*bb)->c_difficulty ? 1:-1;
}
int cmpCDD(const void* a, const void* b)
{
    course** aa = (course**)a;
    course** bb = (course**)b;
    return (*bb)->c_difficulty > (*aa)->c_difficulty ? 1:-1;
}
int cmpIQA(const void* a, const void* b)
{
    course** aa = (course**)a;
    course** bb = (course**)b;
    return (*aa)->i_quality > (*bb)->i_quality ? 1:-1;
}
int cmpIQD(const void* a, const void* b)
{
    course** aa = (course**)a;
    course** bb = (course**)b;
    return (*bb)->i_quality > (*aa)->i_quality ? 1:-1;
}

course** sort_four(int dir, int kind, course** courses, int num)  // dir: 1 for ascend, 2 for descend, kind: 2 for c_quality, 3 for c_difficulty, 4 for i_quality
{
    course** sorted = (course**)malloc(sizeof(course*)*(num+1));
    if(!sorted)
    {
        perror("Error malloc space");
        exit(-1);
    }
    int i;
    for(i=0; i<(num+1); i++)
    {
        sorted[i] = courses[i];
    }
    if(kind==1)
    {
        if(dir==1) qsort(sorted, num, sizeof(sorted[0]), cmpERA);
        else if(dir==2) qsort(sorted, num, sizeof(sorted[0]), cmpERD);
    }
    else if(kind==2)
    {
        if(dir==1) qsort(sorted, num, sizeof(sorted[0]), cmpCQA);
        else if(dir==2) qsort(sorted, num, sizeof(sorted[0]), cmpCQD);
    }
    else if(kind==3)
    {
        if(dir==1) qsort(sorted, num, sizeof(sorted[0]), cmpCDA);
        else if(dir==2) qsort(sorted, num, sizeof(sorted[0]), cmpCDD);
    }
    else if(kind==4)
    {
        if(dir==1) qsort(sorted, num, sizeof(sorted[0]), cmpIQA);
        else if(dir==2) qsort(sorted, num, sizeof(sorted[0]), cmpIQD);
    }
    return sorted;
}

float avg_four(int kind, course** courses, int num)  // kind: 2 for c_quality, 3 for c_difficulty, 4 for i_quality
{
    float tot = 0.0;
    int i;
    for(i=0; i<num; i++)
    {
        if(kind==1)
        {
            tot += courses[i]->enroll;
        }
        if(kind==2)
        {
            tot += courses[i]->c_quality;
        }
        else if(kind==3)
        {
            tot += courses[i]->c_difficulty;
        }
        else if(kind==4)
        {
            tot += courses[i]->i_quality;
        }
    }
    return tot/num;
}

course* max_four(int kind, course** courses, int num)  // kind: 1 for enroll, 2 for c_quality, 3 for c_difficulty, 4 for i_quality
{
    course* res;
    int i, max=0;
    float maxf=0.0;
    for(i=0; i<num; i++)
    {
        if(kind==1)
        {
            if(courses[i]->enroll >= max)
            {
                max = courses[i]->enroll;
                res = courses[i];
            }
        }
        else if(kind==2)
        {
            if(courses[i]->c_quality >= maxf)
            {
                maxf = courses[i]->c_quality;
                res = courses[i];
            }
        }
        else if(kind==3)
        {
            if(courses[i]->c_difficulty >= maxf)
            {
                maxf = courses[i]->c_difficulty;
                res = courses[i];
            }
        }
        else if(kind==4)
        {
            if(courses[i]->i_quality >= maxf)
            {
                maxf = courses[i]->i_quality;
                res = courses[i];
            }
        }
    }
    return res;
}

course* min_four(int kind, course** courses, int num)  // kind: 1 for enroll, 2 for c_quality, 3 for c_difficulty, 4 for i_quality
{
    course* res;
    int i, min=1000;
    float minf=1000.0;
    for(i=0; i<num; i++)
    {
        if(kind==1)
        {
            if(courses[i]->enroll <= min)
            {
                min = courses[i]->enroll;
                res = courses[i];
            }
        }
        else if(kind==2)
        {
            if(courses[i]->c_quality <= minf)
            {
                minf = courses[i]->c_quality;
                res = courses[i];
            }
        }
        else if(kind==3)
        {
            if(courses[i]->c_difficulty <= minf)
            {
                minf = courses[i]->c_difficulty;
                res = courses[i];
            }
        }
        else if(kind==4)
        {
            if(courses[i]->i_quality <= minf)
            {
                minf = courses[i]->i_quality;
                res = courses[i];
            }
        }
    }
    return res;
}

char** tostring(course** courses, int num)
{
    char** res = (char**)malloc(sizeof(char*)*(num+1));
    if(!res)
    {
        perror("Error malloc space");
        exit(-1);
    }
    int i;
    for(i=0; i<(num+1); i++)
    {
        res[i] = NULL;
    }
    for(i=0; i<num; i++)
    {
        char* c_info = (char*)malloc(sizeof(char)*120);
        memset(c_info,0,120);
        if(!c_info)
        {
            perror("Error malloc space");
            exit(-1);
        }
        sprintf(c_info, "%s-%d-%s\t%-30s\t%d\t%.2f\t%.2f\t%.2f", (courses[i])->major, (courses[i])->c_num, (courses[i])->c_subnum, (courses[i])->instructor, (courses[i])->enroll, (courses[i])->c_quality,(courses[i])->c_difficulty, (courses[i])->i_quality);
        res[i] = c_info;
    }
    return res;
}

char* print_table(course** courses, int num)
{
    char* table = (char*)malloc(sizeof(char) * 160 * num);
    memset(table, 0, 160 * num);
    int i;
    for(i = 0; i< num; i++)
    {
        strcat(table, "<tr>\n");
        /*Column one course*/
        char course[20] = {'\0'};
        sprintf(course, "%s-%d-%s", (courses[i])->major, (courses[i])->c_num, (courses[i])->c_subnum);
        strcat(table, "<td>");
        strcat(table, course);
        strcat(table, "</td>\n");
        /*Column two Instructor*/
        char instructor[30] = {'\0'};
        sprintf(instructor, "%s",courses[i]->instructor);
        strcat(table, "<td>");
        strcat(table, instructor);
        strcat(table, "</td>\n");
        /*Column three Enroll*/
        char enroll[3] = {'\0'};
        sprintf(enroll, "%d",courses[i]->enroll);
        strcat(table, "<td>");
        strcat(table, enroll);
        strcat(table, "</td>\n");
        /*Column four course quality*/
        char c_quality[5] = {'\0'};
        sprintf(c_quality, "%.2f",courses[i]->c_quality);
        strcat(table, "<td>");
        strcat(table, c_quality);
        strcat(table, "</td>\n");
        /*Column five Instructor Quality*/
        char i_quality[5] = {'\0'};
        sprintf(i_quality, "%.2f",courses[i]->i_quality);
        strcat(table, "<td>");
        strcat(table, i_quality);
        strcat(table, "</td>\n");
        /*Column six course difficulty*/
        char c_difficulty[5] = {'\0'};
        sprintf(c_difficulty, "%.2f",courses[i]->c_difficulty);
        strcat(table, "<td>");
        strcat(table, c_difficulty);
        strcat(table, "</td>\n");
        //postprocessing
        strcat(table, "</tr>\n");
    }
    return table;
}

void free_char(char** res)
{
    int i;
    for(i=0; res[i]; i++)
    {
        free(res[i]);
    }
    free(res);
}

void free_courses_all(course** courses)
{
    int i;
    for(i=0; courses[i]; i++)
    {
        free(courses[i]);
    }
    free(courses);
}
