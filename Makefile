CC=g++
RM=rm -vf
CPPFLAGS=-std=c++11 -Wall -pedantic -Wextra
.PHONY: all clean
all: client
client: client.cpp
		${CC} ${CPPFLAGS} client.cpp -o chat_client
clean:
		${RM} *.o
