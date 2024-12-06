/*
 * lookup8 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet TCP Sockets
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
    static struct hostent * host;
	static int first_time = 1;

	if (first_time) {        /* connect to socket ; resource is server name */
		first_time = 0;

		/* Set up destination address. */
		/* Fill in code. */
        host = gethostbyname(resource);
		server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = inet_addr(inet_ntoa( *((struct in_addr *)host->h_addr) ));
        //server.sin_addr.s_addr = inet_addr("127.0.0.1");

		/* Allocate socket.
		 * Fill in code. */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

		/* Connect to the server.
		 * Fill in code. */
        if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
            return UNAVAIL;
	}

	/* write query on socket ; await reply
	 * Fill in code. */
    if (send(sockfd, (void *)sought, sizeof(Dictrec), 0) != sizeof(Dictrec)) {
        DIE("send");
    }

    if (recv(sockfd, (void *)sought, sizeof(Dictrec), 0) < 0) {
        DIE("recv");
    }

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
