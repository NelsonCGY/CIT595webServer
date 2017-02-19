CC = clang
ARGS = -Wall

all: readfile

backdata.o: backdata.c backdata.h
	$(CC) -c backdata.c

readfile: backdata.o testdata.c
	$(CC) -o readfile $(ARGS) backdata.o testdata.c
	
clean: 
	rm -rf readfile *.o

