
#include <iostream>		// cout, cerr, etc
#include <stdio.h>		// perror
#include <string.h>		// bcopy
#include <netinet/in.h>		// struct sockaddr_in
#include <unistd.h>		// read, write, etc
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <fcntl.h>
using namespace std;

const int BUFSIZE=102400;

// These describe the connections to the server
// They are delcared global so that more than one
// function can see them/
int numConnects = 0;	// Number of valid connections
int connection[100];	// the connections
char name[100][30];     // Holds 100 names, each maximum of 30 long.




// Deletes and entry in the connnections array
// by moving every other element over one
// Decrements numConnects
void RemoveClient(int target) {
	close(connection[target]);
	cout << "Client removed.\n";
	for(int i = target; i < numConnects; i++) {
		connection[i] = connection[i+1];
		strcpy(name[i], name[i+1]);
	}
	numConnects--;
}

void broadcast(char *msg)
{
	//char msg_no_carriage_return[100];
	//strcpy(msg_no_carriage_return, msg);
	//msg_no_carriage_return[strlen(msg)-1] = 0;
	for(int target = 0; target < numConnects; target++) {
		int len = write(connection[target], msg, strlen(msg));
		cout << "Sending \"" << msg << "\" to client " << target << endl;
		if (len == -1) {
			RemoveClient(target);
			continue;
		}
	}
}


int MakeServerSocket(char *port) {
	const int MAXNAMELEN = 255;
	const int BACKLOG = 3;	
	char localhostname[MAXNAMELEN]; // local host name
	int s; 		
	int len;
	struct sockaddr_in sa; 
	struct hostent *hp;
	struct servent *sp;
	int portnum;	
	int ret;

	gethostname(localhostname,MAXNAMELEN);
	hp = gethostbyname(localhostname);

	sscanf(port, "%d", &portnum);
	if (portnum ==  0) {
		sp=getservbyname(port, "tcp");
		portnum = ntohs(sp->s_port);
	}
	sa.sin_port = htons(portnum);

	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;

	s = socket(hp->h_addrtype, SOCK_STREAM, 0);

	ret = bind(s, (struct sockaddr *)&sa, sizeof(sa));
	if (ret == -1)
	{
		perror("Making the socket");
		exit(1);
	}

	listen(s, BACKLOG);
	cout << "Waiting for connection on port " << port << endl;
	return s;
}

int main(int argc, char *argv[]) {


	int sock; 		// socket descriptor
	int len;		// length of reveived data
	char buf[BUFSIZE];	// buffer in which to read
	int ret;		// return code from various system calls
	struct sockaddr_in sa;  // Where they connect from
	int sa_len = sizeof(sa);// Length of the address
	fd_set rfds;		// Set of fd's we care about
	int i;
	int max;		// Maximum of all the fd's
	char msg[BUFSIZE+100];	// The reply

	sock = MakeServerSocket(argv[1]);
	if (sock == -1) {
		perror("Bad socket");
		exit(1);
	}
	// After this next line, nothing will block.  
	// Not even read or accept.
	ret = fcntl(sock, F_SETFL, O_NONBLOCK);
	if (ret == -1) {
		perror("Bad fcntl");
		exit(2);
	}
	while (1) {
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		max = sock;
		for(i = 0; i < numConnects; i++) {
			FD_SET(connection[i], &rfds);
			if (max < connection[i]) max = connection[i];
		}
		ret = select(max+1, &rfds, 0, 0, 0);
		if (FD_ISSET(sock, &rfds)) {
			cout << "Someone's trying to connect to me!\n";
			// Accept the connection
			connection[numConnects] = accept(sock, (struct sockaddr *)&sa, (unsigned int *)&sa_len);
			strcpy(name[numConnects], "Unknown");
			if ( connection[numConnects] == -1) {
				perror("Bad accept");
				// exit(3);
			}
			numConnects++;
			cout << "Connection:  numConnects = " << numConnects << endl;
		}
		else {
			// Must be data to read
			for(i = 0; i < numConnects; i++) {
				if (FD_ISSET(connection[i], &rfds)) {
					len = read(connection[i], buf, BUFSIZE-1);
					cout << "Someone sent me data.\n";
					if (len < 1) {
						RemoveClient(i);
						continue;
					}
					else if (len > 4 && buf[0] == 'N' && buf[1] == 'A' && buf[2]=='M' && buf[3]=='E')
					{
						cout << "Someone's sent me a name change\n";
						buf[len-2] = 0;  // remove the trailing "\r\n"
						strcpy(name[i], buf+4);
						sprintf(msg, "Client %d now named %s\n", i, name[i]);
						broadcast(msg);
					}
					else if (len > 4 && buf[0] == 'Q' && buf[1] == 'U' && buf[2]=='I' && buf[3]=='T')
					{
						cout << "Someone sent me a QUIT command.\n";
						strcpy(name[i], buf+4);
						sprintf(msg, "Removing Client %d\n", i);
						broadcast(msg);
						RemoveClient(i);
					}
					else {
						cout << "I'll broadcast it\n";
						buf[len] = 0;  // remove the trailing "\r\n"
						sprintf(msg, "Client %s: %s\n", name[i], buf);
						broadcast(msg);
					}
				}
			}
		}
	}
}