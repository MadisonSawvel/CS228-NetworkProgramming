
//Madison Sawvel

#include <sys/poll.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <iostream>          
#include <stdio.h>     
#include <string.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <stdlib.h> 

using namespace std;

void ErrorCheck(const char *msg){
        perror(msg);
        exit(1);
}

int MakeSocket(char *host, char *port) {
    int s;                  
    int len;        
    struct sockaddr_in sa; 
    struct hostent *hp;
    struct servent *sp;
    int portnum;    
    int ret;

    hp = gethostbyname(host);
    if (hp == 0){
        ErrorCheck("Gethostbyname");
    }
    bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;
    sscanf(port, "%d", &portnum);
    if (portnum > 0) {
        sa.sin_port = htons(portnum);
    }
    else {
        sp=getservbyname(port, "tcp");
        portnum = sp->s_port;
        sa.sin_port = sp->s_port;
    }
    s = socket(hp->h_addrtype, SOCK_STREAM, 0);
    if(s == -1){
        ErrorCheck("Could not make socket");
    }
    ret = connect(s, (struct sockaddr *)&sa, sizeof(sa));
    if(ret == -1){
        ErrorCheck("Not able to connect");
    }
    cout << "Connect to host " << host  << " port " << port << endl;
    return s;
}

int main(int argc, char * argv[]){

    int socket = MakeSocket(argv[1], argv[2]);
    if(socket == -1){
        ErrorCheck("Socket not made");
    }
    cout << "The host: " << argv[1] << " the port: " << argv[2] << endl;

    struct pollfd poll_list[2];
    poll_list[0].fd = socket; 
    poll_list[0].events = POLLIN;
    poll_list[1].fd = 0;
    poll_list[2].events = POLLIN;

    while(1 == 1){
        char c;
        int error = poll(poll_list, 2, -1);
        if(poll_list[1].revents & POLLIN){
            int r = read(poll_list[1].fd, &c, 1);
            if( r < 1){
                exit(1);
            }
            int w = write(poll_list[0].fd, &c, 1);
            //cout << "Read from keyboard: " << r << " Wrote to server: " << w << endl;
        }
        if(poll_list[0].revents & POLLIN){
            int r = read(poll_list[0].fd, &c,1);
            if (r < 1){
                exit(1);
            }
            int w = write(poll_list[1].fd, &c,1);
            //cout << "Read from server: " << r << " Wrote to keyboard: " << w << endl;
        }
    }
}
