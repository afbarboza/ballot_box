#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "data.h"

#define MAXLEN			0xFF
#define	MAX_PENDING_CONNECTIONS	10
#define	SERVER_PORT		40011

__attribute__((noinline, noreturn))
void handle_error(void)
{
	char *errmsg = NULL;
	errmsg = strerror(errno);
	perror(errmsg);
	exit(EXIT_FAILURE);
}

void *init_msg_xcgh(void *socket_descriptor)
{
	char rcv_data = 0;
	int connected = 0, i = 0;
	connected = (int) socket_descriptor;

	/* warns about a new connecion thread */
	printf("new connection id: %d\n", connected);

	/* keep up the protocol exchange between server and client */
	while (1) {

		/* protocol 1: send the list of candidates */
		send(connected, candidates, (N_CANDIDATES * sizeof(vote_t)), 0);
		break;
	}

	printf("closing down connection id: %d\n", connected);
	pthread_exit(EXIT_SUCCESS);
	return NULL;
}

int main(void)
{
	char snd_data, rcv_data, flag = 1;
	int sock, bytes_recv, i;
	struct sockaddr_in client_addr;

	/* load list of all candidates in server */
	load_candidates();

	/* handles a invalid try to write to a unconnected socket */
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		handle_error();
	}

	/* create a socket descriptor */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		handle_error();
	}

	/* allows multiple connections per client */
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
		handle_error();
	}

	/* setup manually the socket destiny address (instead of inquire kernel) */
	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(SERVER_PORT),
		.sin_addr.s_addr = INADDR_ANY,
		.sin_zero = {0, 0, 0, 0, 0, 0, 0, 0}
	};

	/* assigns an address to the socket */
	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1) {
		handle_error();
	}

	/* makes this socket accept incoming connection requests */
	if (listen(sock, MAX_PENDING_CONNECTIONS) == -1) {
		handle_error();
	}

	printf("> server is waiting for connections at %d\n", SERVER_PORT);

	/* wait for connection requests */
	while (1) {

		/* accept the connection request and creates a new connected socket */
		socklen_t sin_size = (socklen_t) sizeof(struct sockaddr_in);
		int connected = accept(sock, (struct sockaddr *) &client_addr, &sin_size);

		/* creates a new thread to handle the new connection */
		pthread_t *server_thread = (pthread_t *) malloc(sizeof(pthread_t));
		pthread_create(server_thread, NULL, init_msg_xcgh, (void *) connected);

		fflush(stdout);
	}

	close(sock);
	exit(EXIT_SUCCESS);
}
