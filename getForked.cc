
//Madison Sawvel

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <cctype>
#include <fcntl.h>
#include <chrono>
#include <sstream>
#include <sys/wait.h>

using namespace std;

const int BUFSIZE = 1024000;

int MakeServerSocket(int port){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}
void timeForServer(int fd, struct hostent *host, struct sockaddr_in sa, char *buf){
    
    cout << "Forked\n";
    time_t *rawtime = new time_t;
    struct tm *timeinfo;
    time(rawtime);
    timeinfo = localtime(rawtime);

    ofstream logfile("TimeAndRequest.txt");
    if (!logfile){
        cout << "Error while creating logfile" << endl;
    }

    int r = read(fd, buf, 9999);
    if (r < 0){
        perror("Could not read request");
        exit(1);
    }

    string req = buf;
    string message = buf;
    string referrer = buf;

    int fromSlash = req.find('/');
    int fromSpace = req.find(' ', fromSlash);
    req.erase(0, fromSlash);
    string filename = req.substr(0, req.find(' ', 0));
    filename = '.' + filename;
    
    logfile << filename;

    message.erase(fromSpace + 11, message.length());

    int referrerStart = referrer.find("sec-ch-ua:");
    referrer.erase(0, referrerStart + 12);
    referrerStart = referrer.find("\"");
    referrer.erase(referrerStart, referrer.length());

    host = gethostbyaddr((const void *)&sa.sin_addr, sizeof(struct in_addr), AF_INET);
    string hostname = host->h_name;


    int w;
    int len;
    int contentLength;

    int file = open(filename.c_str(), O_RDONLY);
    if (file == -1){
        file = open("./404.txt", O_RDONLY);
        w = write(fd, "HTTP/1.0 404 NOT OK\r\n\r\n", 23);
        if (w <= 0){
            perror("Could not write to file");
            close(fd);
        }
        do{
            len = read(file, buf, 9999);
            if (len < 0){
                perror("Could not read file");                        
                close(fd);
            }
            w = write(fd, buf, len);
            if (w < 0){
                perror("Could not write");
                close(fd);
            }
            close(fd);
        } while (len > 0);
    }
    else{
        w = write(fd, "HTTP/1.0 200 OK\r\n\r\n", 19);
        cout <<"w: " << w << endl;
        if (w < 0){
            perror("Write was bad sending file");
            close(fd);
        }
        do{
            len = read(file, buf, 9999);
            if (len < 0){
                perror("Could not read file being sent");
                close(fd);
            }
            //buf[len] = 0;
            contentLength += len;
            cout << "len: " << len << endl;
            w = write(fd, buf, len);
            if (w < 0){
                perror("Write was bad after reading");
                close(fd);
            }
        } while (len > 0);
    }
    logfile << hostname << " - - [" << asctime(timeinfo) << "] " << req << " 200 "
            << "ContentLength: " << contentLength << " - " << referrer << "\n";
    close(fd);
    exit(1);

}

int main(int argc, char *argv[]){

    int s;
    int len;
    char buf[BUFSIZE];
    int ret;
    struct hostent *host;

    s = MakeServerSocket(8008);

    while (1 == 1){
        struct sockaddr_in sa;
        int sa_len = sizeof(sa);
        int fd = accept(s, (struct sockaddr *)&sa, (unsigned int *)&sa_len);
        cout << "Connection:  " << endl;

        int pid = fork();
        if (pid == -1){
            perror("Unable to fork");
            exit(1);
        }
        else if (pid > 0){ //parent
            close(fd);
            int w;
            do{
                w = waitpid(pid, NULL, WNOHANG);
            } while (w > 0);
            cout << "After waitpid - w: "<< w << "\n";

        }
        else{ //child
            timeForServer(fd, host, sa, buf); 
            close(fd);
            exit(0);
        }
    }
}
