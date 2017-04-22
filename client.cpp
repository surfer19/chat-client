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

char logbuf[MAXMSG];  //send inital msg GLOBAL VARIABLE
int sd, pid; ///global vars
int sig_handler_used = 0;

void exit_error(char* msg) {
    perror(msg);
    exit(1);
}

void KBInterrupt(int sig) {
    fprintf(stdout,"Exit!\n");
    send(sd, &logbuf, sizeof(logbuf), 0);
    exit(1);
}

void sig_handler(int signo)
{
    if (signo == SIGINT) {

        //printf("received SIGINT\n");
        // send message about disc
        send(sd, &logbuf, sizeof(logbuf), 0);
        if (pid == 0) {
            exit(0); //exit son
        }
        sig_handler_used = 1;
        kill(pid,SIGKILL); // kill son
        wait(NULL); // wait for killing
        shutdown(sd,2);
        close(sd);
        exit(0);
    }
}


int main (int argc, char const *argv[]) {
    // inits
    int port, n, send_val, read_val;
    string nick_name;
    struct hostent *phe;
    struct sockaddr_in struct_sock;
    char readbuf[MAXMSG]; //read
    char sendbuf[MAXMSG]; //send

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

    /*
     *
     * catch signal things
     *
     */
    // Variables
    fd_set fd;
    int charsRead;
    char buff[256];
    struct sigaction handler;

    // Setup Action Handler
    handler.sa_handler = KBInterrupt; // Function to call
    if (sigfillset(&handler.sa_mask) < 0)
        exit_error("sigfillset failed");
    handler.sa_flags=0;
    if (sigaction(SIGINT,&handler,0) < 0) // Setup signal
        exit_error("sigaction failed");

    // Setup file descriptors
    FD_ZERO(&fd);
    FD_SET(STDIN_FILENO,&fd);



    memset(sendbuf, '\0', sizeof(sendbuf));
    memset(readbuf, '\0', sizeof(readbuf));
    memset(logbuf, '\0', sizeof(logbuf));

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
     *  disconnect and bye message
     *
     */


    memset(logbuf, '\0', sizeof(logbuf));
    // concatenate
    conc = nick_name + " logged out" + "\r\n";
    strcpy(logbuf, conc.c_str());

    /*
     *
     * FORK process
     *
     */

    pid = fork(); // create son
//|| signal(SIGINT,sig_handler) !=  SIG_ERR
    while ((send_val && read_val)) {
        //recv
        if (pid == 0) //son
        {
            if ((read_val = read(sd, &readbuf, sizeof(readbuf)))) {
                cout << readbuf;
                // clear buffer
                memset(readbuf, '\0', sizeof(readbuf));
            }
//            if (signal(SIGINT, sig_handler) == SIG_ERR){
//                printf("\ncan't catch SIGINT\n");
//            }
            select(STDIN_FILENO+1,&fd,NULL,NULL,NULL);
            if(FD_ISSET(STDIN_FILENO,&fd)) { // Checks if enter has been pressed
                charsRead = read(STDIN_FILENO,buff,255);
                write(STDOUT_FILENO,buff,charsRead);
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
                conc_string.push_back('\0');
                // do copy of concatanate string to empty buf
                strcpy(sendbuf, conc_string.c_str());
                // send final message
                send(sd, sendbuf, sizeof(sendbuf), 0);
                // clear buffer
                memset(sendbuf, '\0', sizeof(sendbuf));

//                if (signal(SIGINT, sig_handler) == SIG_ERR){
//                    printf("\ncan't catch SIGINT\n");
//                }
            }
        }
    }


    return 0;

}