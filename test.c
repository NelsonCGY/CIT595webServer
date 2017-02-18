#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
int main(){
	char* a = malloc(sizeof(char) * 7);
	strcat(a, "scd");
	printf("%s", a);
}