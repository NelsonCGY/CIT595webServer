CC = clang
ARGS = -Wall

all: test requests httpserver

backdata.o: backdata.c backdata.h
	$(CC) -c backdata.c

test: backdata.o testdata.c
	$(CC) -o test $(ARGS) backdata.o testdata.c
	
requests: backdata.o requests.c
	$(CC) -o requests $(ARGS) backdata.o requests.c

httpserver: backdata.o httpserver.c
	$(CC) -lpthread -o httpserver $(ARGS) backdata.o httpserver.c

clean: 
	rm -rf test requests httpserver *.o

