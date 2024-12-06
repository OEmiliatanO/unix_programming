/*
 * lookup7 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Unix TCP Sockets
 * The name of the socket is passed as resource.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_un server;
	static int first_time = 1;
    static Dictrec end;

	if (first_time) {     /* connect to socket ; resource is socket name */
		first_time = 0;

        memset(&end, -1, sizeof(end));

		/* Set up destination address.
		 * Fill in code. */
        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockfd == -1) {
            DIE("socket");
        }

		/* Allocate socket. */
		server.sun_family = AF_UNIX;
		strcpy(server.sun_path,resource);

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

    send(sockfd, (void *)sought, 0, 0);

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
