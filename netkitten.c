#include <stdio.h>
#include <arpa/inet.h> /* inet_pton() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <unistd.h> /* close(), read() */
#include <stdlib.h> /* exit() */
#include <netdb.h> /* getaddrinfo() */
#include <string.h>
//#include <sys/types.h> /* not required on linux, see man socket */
//#include <sys/socket.h> /* struct sockaddr, included by netinet/in.h */

#define VERBOSE 1

	
int main(int argc, char *argv[])
{
 	if ( argc != 3 ) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(1);
	}

	int mysock;
	struct sockaddr_in myaddr;
	char buf[2048];
	int n;

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(atoi(argv[2]));
	// set myaddr.sin_addr.s_addr
	// see http://www.retran.com/beej/sockaddr_inman.html
	//if ( inet_pton(AF_INET,argv[1], &myaddr.sin_addr) != 1 ){
	//	fprintf(stderr, "Failed to set IP address\n");
	//	exit(1);
	//}
	myaddr.sin_addr.s_addr = inet_addr(argv[1]);
	mysock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if 
	( connect(mysock, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1 ){
		fprintf(stderr, "Failed to connect to %s %s\n", argv[1], 
								argv[2]);
		exit(1);
	}

	pid_t pid = fork();
	if (pid == 0){ //Child thread to read from the net
		while ( (n = read(mysock, buf, sizeof(buf))) > 0){
			if (VERBOSE)
				fprintf(stderr,"\n[%d]>", n);
			write(1, buf, n);
                        fflush(stdout); // permanently: setbuf(stdout, NULL);
		}

	}
	else { //Parent thread to read from stdin(0)
		while (	(n = read(0, buf, sizeof(buf))) > 0 ){
			send(mysock, buf, n, 0);
 		}
	}

	close(mysock);
	return 0;
}
