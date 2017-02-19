/* a head file with functions implemented for basic data process - gongyao */

typedef struct Course{
    char major[5];
    int c_num;
    char c_subnum[5];
    char instructor[30];
    int enroll;
    float c_quality;
    float c_difficulty;
    float i_quality;
}course; // structure for a single course

course** readfile(char* filename, int* total); // reads a course_evals file and return the course information with a structure list and total number

course** find_course(char* major, int c_num, course** courses, int* num); // find courses with major or course number, return the list of courses and total number
course** find_instructor(char* instructor, course** courses, int* num); // find courses with instructor, return the list of courses and total number

course** filter_enroll(int dir, int k, course** courses, int* num); // filter courses with enroll number, return the list of courses and total number
course** filter_three(int dir, int kind, float k, course** courses, int* num);// filter courses with one of three course scores, return the list of courses and total number

course** sort_enroll(int dir, course** courses, int num); // sort courses with enroll number, return the list of sorted courses
course** sort_three(int dir, int kind, course** courses, int num); // sort courses with one of three course scores, return the list of sorted courses

int avg_enroll(course** courses, int num); // calculate average enroll numbers, return the result by int
float avg_three(int kind, course** courses, int num); // calculate one of three average course scores, return the result by float

course* max_four(int kind, course** courses, int num); // find the maximum of one of four course info, return the max course
course* min_four(int kind, course** courses, int num); // find the minimum of one of four course info, return the max course

char** tostring(course** courses, int num); // make a list of courses to a list of output string
void free_char(char** res); // clear string list
void free_courses_all(course** courses); // clear courses list and inner course information
