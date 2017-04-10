#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include <string>
#include <iostream>
using namespace std;

#define MAXMSG 1024

int main (int argc, char const *argv[]) {
    // inits
    int port, n, sd, send_val, read_val;
    string nick_name;
    struct hostent *phe;
    struct sockaddr_in struct_sock;
    char readbuf[MAXMSG]; //read
    char sendbuf[MAXMSG]; //send
    char logbuf[MAXMSG];  //send inital msg
    string conc_string;
    string s_sendbuf;

    if (argc != 5) {
        fprintf(stderr, "too less arguments, use: client ip port\n");
        exit(1);
    }
    //get host address via second argument
    if (!(phe = gethostbyname(argv[2]))) {
        fprintf(stderr, "bad host address: %s\n", argv[2]);
        exit(1);
    }
    //read port and its length
    // TODO port is 21011
    if (sscanf("21011", "%d%n", &port, &n) != 1) {
        fprintf(stderr, "bad port number: %d\n", port);
        exit(1);
    }
    nick_name = argv[4];

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct_sock.sin_family = AF_INET;
    struct_sock.sin_port = htons(port);
    memcpy(&struct_sock.sin_addr, phe->h_addr_list[0], sizeof(struct_sock.sin_addr));

    if (connect(sd, (struct sockaddr *) &struct_sock, sizeof(struct_sock)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }


    send_val = 1;
    read_val = 1;

    /*
     *
     *  send initial message after login user
     *
     */

    string conc = nick_name + " logged in" + "\r\n";
    strcpy(logbuf, conc.c_str());
    send(sd, &logbuf, sizeof(logbuf), 0);

    /*
     *
     * FORK process
     *
     */

    int pid = fork(); // create son

    while (send_val && read_val) {
        //recv
        if (pid == 0) //son
        {
            if ((read_val = read(sd, &readbuf, sizeof(readbuf)))) {
                cout << readbuf;
                // clear buffer
                memset(readbuf, '\0', sizeof(readbuf));
            }
        }
        else { //send | father
            if ((send_val = read(0, &sendbuf, sizeof(sendbuf)))) {
                // copy buf to string
                s_sendbuf = sendbuf;
                // clear buf
                memset(sendbuf, '\0', sizeof(sendbuf));
                // concatenate strings
                conc_string = nick_name + ": " + s_sendbuf;
                conc_string.pop_back();
                conc_string += "\r\n";
                // do copy of concatanate string to empty buf
                strcpy(sendbuf, conc_string.c_str());
                // send final message
                send(sd, sendbuf, sizeof(sendbuf), 0);
                // clear buffer
                memset(sendbuf, '\0', sizeof(sendbuf));
            }
        }
    }

    /*
     *
     *  disconnect and bye message
     *
     */

    if (pid == 0) {
        exit(0); //exit son
    }

    memset(logbuf, '\0', sizeof(logbuf));
    // concatenate
    conc = nick_name + " logged out" + "\r\n";
    strcpy(logbuf, conc.c_str());
    // send message about disc
    send(sd, &logbuf, sizeof(logbuf), 0);

    kill(pid,SIGKILL); // kill son
    wait(NULL); // wait for killing
    shutdown(sd,2);
    close(sd);

    return 0;

}