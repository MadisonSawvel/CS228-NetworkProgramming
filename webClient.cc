
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
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex>
#include <algorithm>
#include <chrono>
#include <cstddef>

using namespace std;
const int BUFSIZE=102400;

void ErrorCheck(bool condition, const char *msg)
{
	if (condition)
	{
		perror(msg);
		exit(1);
	}
}

int MakeSocket(const char *host, const char *port) {
	int s; 			
	int len;	
	struct sockaddr_in sa; 
	struct hostent *hp;
	struct servent *sp;
	int portnum;	
	int ret;

	hp = gethostbyname(host);
	ErrorCheck(hp==0, "Gethostbyname: it says success but your URL was actual garbage. ");
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
	ErrorCheck(s == -1, "Could not make socket");
	ret = connect(s, (struct sockaddr *)&sa, sizeof(sa));
	ErrorCheck(ret == -1, "Could not connect");
	return s;
}
void send(int sock, string wmes){ 
	cout << wmes << endl;
	const void* w = wmes.c_str();
	int smes = write(sock,w,wmes.length());
	ErrorCheck(smes == -1, "Write was bad.\r\n");
}	
int main(int argc, char** argv){
    char buf[BUFSIZE];
    
	cout << "I'm forcing you to use my page." << endl;
	string url = "http://euclid.nmu.edu/~eseymour/duck.txt";

	size_t found = url.find_first_of(":"); 
	string protocol = url.substr(0,found);

	string new_url = url.substr(found+3); 
	size_t found1 = new_url.find_first_of("/"); 
	string hostname = new_url.substr(0,found1); 

	size_t found2 = new_url.find_first_of("/");
	string filename = new_url.substr(found2);

    int sock = MakeSocket(hostname.c_str(), "80");
	cout << "socket is " << sock << endl;
	assert(sock != -1);

	std::chrono::time_point<std::chrono::steady_clock> start, end; 
	start = std::chrono::steady_clock::now();

	send(sock,"GET " + filename + " HTTP/1.0\r\n\r\n"); 
	do {
		int rmes = read(sock,buf,BUFSIZE-1);
		assert(rmes >= 0);
		buf[rmes] = 0;
		if(buf[9] != '2' && buf[9] != '3'){ 
			cout << "The response was not a 2 or 3, resend\n";
		}
		cout << buf << endl;
	} while(rmes > 0);

	end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	cout << "Time to receive: " << elapsed_seconds.count() << "s\r\n";
	cout << "This is Walter :) " << endl;

	close(sock);
}
