CC = clang
ARGS = -Wall

all: readfile

backdata.o: backdata.c backdata.h
	$(CC) -c backdata.c

readfile: backdata.o
	$(CC) -o readfile $(ARGS) backdata.o
	
clean: 
	rm -rf readfile *.o

