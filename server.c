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


//str MUST BE in json format
void send_vote_files(int socket, char *str_file)
{
	int i;
	char c;
	int len = 0;

	if (str_file == NULL) {
		fprintf(stderr, "error: null pointer at %s:%d\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	len = strlen(str_file);
	for (i = 0; i < len; i++) {
		send(socket, &(str_file[i]), sizeof(char), 0);
	}
}

char *recv_vote_files(int socket)
{
	char c;
	int i = 0;
	char *buffer = NULL;

	/* consumes the received string from java client */
	while (1) {
		recv(socket, &c, sizeof(char), 0);
		buffer = (char *) realloc(buffer, (i + 1) * sizeof(char));
		buffer[i] = c;
		i++;
		if (c == '\n' || c == '\0') {
			break;
		}
	}

	/* just some C bullshit */
	buffer = (char *) realloc(buffer, (i + 1) * sizeof(char));
	buffer[i] = '\0';

	return buffer;
}

void *init_msg_xcgh(void *socket_descriptor)
{
	int client_opcode = 0;
	int connected_sock = 0;
	connected_sock = (int) socket_descriptor;

	/* stores the incoming ballot from client */
	vote_t *buffer_ballot = (vote_t *) malloc(N_CANDIDATES * sizeof(vote_t));

	/* warns about a new connecion thread */
	printf("new connection id: %d\n", connected_sock);

	/* keep up the protocol exchange between server and client */
	while (1) {
		/* protocol: read the command opcode from client */
		recv(connected_sock, &client_opcode, sizeof(int), 0);

		switch (client_opcode) {
			case LD_CANDIDATES:
				/* protocol: send the list of candidates */
				send(connected_sock, server_candidates, (N_CANDIDATES * sizeof(vote_t)), 0);
				break;

			case HLT_BALLOT:
				/* protocol: read the votes coming from the client and count it */
				recv(connected_sock, buffer_ballot, N_CANDIDATES * sizeof(vote_t), 0);
				count_amount_votes(server_candidates, buffer_ballot);
				goto end_connection;
				break;
			
			case PANIC: /* handles when client press Ctrl+C */

				/* protocol: read the votes coming from the client and count it */
				recv(connected_sock, buffer_ballot, N_CANDIDATES * sizeof(vote_t), 0);

				/* alows communication with client exits gracefully */
				puts("panic : client says goodbye...\n");

				goto end_connection;
				break;

			default:
				fprintf(stderr, "bad opcode");
		};
	}


end_connection:
	/* shows the partial results of the ellections */
	rank_server_candidates();

	free(buffer_ballot);
	printf("closing down connection id: %d\n", connected_sock);
	pthread_exit(EXIT_SUCCESS);
	return NULL;
}

int main(void)
{
	char flag = 1;
	int sock;
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
