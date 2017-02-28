#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backdata.h"

/* data function tests - gongyao */
int main()
{
    int total, i;
    course** courses = readfile("course_evals.txt", &total);
    printf("total %d\n", total);

    // tostring test
    /*char** res = tostring(courses,total);
    for(i=0;i<total;i++){
        //printf("%s-%d-%s, %s, %d, %.2f, %.2f, %.2f, %d\n", (courses[i])->major, (courses[i])->c_num, (courses[i])->c_subnum, (courses[i])->instructor, (courses[i])->enroll, (courses[i])->c_quality,(courses[i])->c_difficulty, (courses[i])->i_quality, i);
        printf("%s, %d, %d\n", res[i], strlen(res[i]),i+1);
    }*/

    // min max test
    /*course* minR = min_four(1, courses, total);
    printf("\nMinR: %s-%d-%s, %s, %d, %.2f, %.2f, %.2f\n", minR->major, minR->c_num, minR->c_subnum, minR->instructor, minR->enroll, minR->c_quality, minR->c_difficulty, minR->i_quality);
    course* maxD = max_four(3, courses, total);
    printf("\nMaxD: %s-%d-%s, %s, %d, %.2f, %.2f, %.2f\n", maxD->major, maxD->c_num, maxD->c_subnum, maxD->instructor, maxD->enroll, maxD->c_quality, maxD->c_difficulty, maxD->i_quality);*/

    // sort tes
    course** sortC = sort_four(1,4,courses,total);
    char** res = tostring(sortC,total);
    for(i=0; i<total; i++)
    {
        printf("%s, %d\n", res[i],i+1);
    }

    // avg test
    printf("\nAvg difficulty %.2f\n", avg_four(3, courses, total));

    // filter test
    /*course** filterC = filter_enroll(2,32,courses,&total);
    course** filterC = filter_three(2,3,2,courses,&total);
    char** res = tostring(filterC,total);
    for(i=0;i<total;i++){
        printf("%s, %d\n", res[i],i+1);
    }*/


    // find test
    //char a[] = "Ben"; course** findC = find_instructor(a, courses, &total);
    //char b[]="cI"; course** findC = find_course(b, 57, courses, &total);
    /*char** res = tostring(findC,total);
    for(i=0;i<total;i++){
        printf("%s, %d\n", res[i],i+1);
    }*/

    // memory free
    free_char(res);
    free(sortC);
    //free(filterC);
    //free(findC);
    free_courses_all(courses);
    return 0;
}
