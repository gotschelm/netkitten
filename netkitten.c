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

	int sfd;
	struct sockaddr_in myaddr;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	char buf[2048];
	int n;

	// pretty much stolen from man getaddrinfo
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; 

	if (getaddrinfo(argv[1], argv[2], &hints, &result) != 0){
		fprintf(stderr, "getaddrinfo failed");
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
			     rp->ai_protocol);
		if (sfd == -1)
			continue;
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */
		close(sfd);
	}	

	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */
	//so far the manpage
	
	pid_t pid = fork();
	if (pid == 0){ //Child thread to read from the net
		while ( (n = read(sfd, buf, sizeof(buf))) > 0){
			if (VERBOSE)
				fprintf(stderr,"\n[%d]>", n);
			write(1, buf, n);
			fflush(stdout); // permanently: setbuf(stdout, NULL);
		}

	}
	else { //Parent thread to read from stdin(0)
		while (	(n = read(0, buf, sizeof(buf))) > 0 ){
			send(sfd, buf, n, 0);
		}
	}

	return 0;
}
