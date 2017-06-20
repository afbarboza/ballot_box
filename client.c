#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "data.h"

#define MAXLEN			0xFF
#define	SERVER_PORT		40011

int sock = 0;
bool connected;

__attribute__((noinline, noreturn))
void handle_error(const int __line)
{
	printf("at %d\n", __line);
	char *errmsg = NULL;
	errmsg = strerror(errno);
	perror(errmsg);
	exit(EXIT_FAILURE);
}

__attribute__((noinline))
void halt_client(int foo)
{
	char snd_data = '0';

	if (connected == true) {
		send(sock, &snd_data, sizeof(snd_data), 0);
		close(sock);
		fflush(stdout);
	}

	puts("client says goodbye..\n");
	exit(EXIT_SUCCESS);
}

int main(void)
{
	char snd_data;
	int bytes_recv, i;
	struct hostent *host = NULL;
	vote_t candidates[N_CANDIDATES];
	char *servername = (char *) malloc(MAXLEN * sizeof(char));

	/* allow comunication with server exit gracefully */
	connected = false;
	signal(SIGINT, halt_client);

	/* reads the servername (IP or hostname) */
	scanf("%s", servername);

	/* gets the server address using the *deprecated* method of probe /etc/hosts */
	host = gethostbyname(servername);
	if (host == NULL) {
		fprintf(stderr, "%s:%d - invalid host name. please try again.\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* create a socket descriptor */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		handle_error(__LINE__);
	}

	/* setup manually the socket destiny address (instead of inquire kernel) */
	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(SERVER_PORT),
		.sin_addr = *((struct in_addr *)host->h_addr),
		.sin_zero = {0, 0, 0, 0, 0, 0, 0, 0}
	};

	/* _only at client side_ : connect a socket to the server */
	if (connect(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1) {
		handle_error(__LINE__);
	}

	/* wait untill apllication terminates */
	while (1) {
		connected = true;

		/*protocol 1: reads the list of candidates from server */
		recv(sock, &candidates, N_CANDIDATES * sizeof(vote_t), 0);
		for (i = 0; i < N_CANDIDATES; i++) {
			printf("%d - %s\n", candidates[i].n_votes, candidates[i].candidate_party);
		}

		break;
	}

	close(sock);
	fflush(stdout);
	free(servername);
	exit(EXIT_SUCCESS);
}
