/*
 * lookup9 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet UDP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {  /* Set up server address & create local UDP socket */
		first_time = 0;

		/* Set up destination address.
		 * Fill in code. */
        host = gethostbyname(resource);
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = inet_addr(inet_ntoa( *((struct in_addr *)host->h_addr) ));

		/* Allocate a socket.
		 * Fill in code. */
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	}

	/* Send a datagram & await reply
	 * Fill in code. */
    socklen_t siz = -1;
    sendto(sockfd, (void *)sought, sizeof(Dictrec), 0, (struct sockaddr *)&server, sizeof(server));
    recvfrom(sockfd, (void *)sought, sizeof(Dictrec), 0, (struct sockaddr *)&server, &siz);

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
