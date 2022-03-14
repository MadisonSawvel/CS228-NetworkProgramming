

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
#include <algorithm>

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
	ErrorCheck(hp==0, "Gethostbyname");
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
void send(int sock, string wmes){ //writes string to server
	cout << wmes << endl;
	const void* w = wmes.c_str();
    //write bytes
	int smes = write(sock,w,wmes.length());
    //Error check
	ErrorCheck(smes == -1, "Write was bad.\r\n");
}
void receive(int sock){ 
	char buf[1000];
	int rmes = read(sock,buf,999);
	assert(rmes >= 0);
	buf[rmes] = 0;
	if(buf[0] != '2' && buf[0] != '3'){ 
		cout << "The response was not a 2 or 3, resend\n";
	}
	cout <<  buf << endl;
}		  
int main(int argc, char** argv){
    char buf[BUFSIZE];
    
    int sock = MakeSocket("alt4.gmail-smtp-in.l.google.com.", "25");
	cout << "socket is " << sock << endl;
	assert(sock != -1);

	string cmd_in;
	cout << "What site would you like to be?" << endl;
	getline(cin,cmd_in);
	string site = cmd_in;

	cout << "What username would you like to be?" << endl;
	 getline(cin,cmd_in);
	 string user = cmd_in;

	cout << "Who do you want to send to?" << endl;
	getline(cin,cmd_in);
	string recip = cmd_in;

	cout << "What is the subject of the email?" << endl;
	getline(cin, cmd_in);
	string subj = cmd_in;

	cout<<"What would you like the body to be?" << endl;
	string line;
	string message;

	while(true){
		getline(cin,line);
		message+=line+"\r\n";
		if(line == "."){
			break;
		}

	}
	string greet = "HELO " + site + "\r\n";
	string mailee = "MAIL FROM:<" + user + "@" + site + "> \r\n";
	string rcpt = "RCPT TO:<" + recip + "> \r\n";
	string subject = "Subject: " + subj + "\r\n";
	string rec = "To: " + recip + "\r\n";
	string gib = "From: " + user + "@" + site + "\r\n";
	  
	receive(sock);
	send(sock, greet);
	receive(sock);

	send(sock,mailee);
	receive(sock);

	send(sock, rcpt);
	receive(sock);

	send(sock,"DATA\r\n");
	receive(sock);

	send(sock,subject);
	send(sock,rec);
	send(sock,gib);
	send(sock, "\r\n");
	send(sock,message);
	receive(sock);

	send(sock, "QUIT \r\n");
	receive(sock);

	close(sock);
}

