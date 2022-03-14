/* WARNING!  THIS CODE DOES NOT WORK FOR REASONABLE YET LARGE  FILE SIZES!!!! */

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

int main(int argc, char *argv[]) {
	char buf[BUFSIZE];

	int sock = MakeSocket(argv[1], argv[2]);
	cout << "socket is " << sock << endl;
	assert(sock != -1);

	int sendmes = write(sock, "NAMEMaddy", 12); //sets name to maddy, thought it was asking for an input
	ErrorCheck(sendmes == -1, "Write was bad.");

	int readmes = read(sock, buf, 999);  //this reads servers response:reads up to 999 bytes from sock starting at buf
	assert(readmes > 0); //only runs if the message is larger than 0
	buf[readmes] = 0; //buf and size of readmes?
	string ans = buf; //the value at buf is made into new string 
	cout << ans << endl;

while(true){

	cout << "What would you like to send? NAME, TEXT, or QUIT?" << endl;

	string cmd_in;
	getline(cin,cmd_in); 

	if((cmd_in[0] == 'q' || cmd_in[0] == 'Q') && (cmd_in[1] == 'u' || cmd_in[1] == 'U') && (cmd_in[2] == 'i' || cmd_in[2] == 'I') && (cmd_in[3] == 't' || cmd_in[3] == 'T')) { //brought to you in part: by the tutor man elliot seymour 
		transform(cmd_in.begin(), cmd_in.end(), cmd_in.begin(), ::toupper);
		const void* q = cmd_in.c_str();
		sendmes = write(sock,q,cmd_in.length() + 3);
		ErrorCheck(sendmes == -1, "Write was bad.");
		
		readmes = read(sock, buf, 999);
		assert(readmes > 0);
		buf[readmes] = 0;
		ans = buf;
		cout << ans << endl;
		break;
		
	}

	const void* c = cmd_in.c_str(); //pointer made
	sendmes = write(sock, c, cmd_in.length() + 3); //writes to server up to the command length + 3(size of sock?) from pointed array to sock
	ErrorCheck(sendmes == -1, "Write was bad.");

	readmes = read(sock, buf, 999);
	assert(readmes > 0); 
	buf[readmes] = 0; 
	ans = buf; 
	cout << ans << endl;
}
	close(sock);
}


