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
	int cmd_exit = 666;

	if (connected == true) {
		send(sock, &cmd_exit, sizeof(int), 0);
		close(sock);
		fflush(stdout);
	}

	puts("client says goodbye..\n");
	exit(EXIT_SUCCESS);
}

int main(void)
{
	int user_vote = 0;
	int cmd_opcode = 0;
	int bytes_recv, i;
	struct hostent *host = NULL;
	client_candidates = (vote_t *) malloc(N_CANDIDATES * sizeof(vote_t));
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

		/* displays main menu of actions */
		output_main_menu();

read_opcode:
		/* gets the selected action */		
		scanf("%d", &cmd_opcode);

		/* sends the command opcode to the server */
		send(sock, &cmd_opcode, sizeof(int), 0);

		/* delays to ensure server receives the opcode */
		usleep(3);

		switch(cmd_opcode) {
			case VOTE:
				get_user_vote();
				break;
			case VOTE_BLANK:
				inc_vote_candidate(3);
				break;
			case VOTE_NULL:
				inc_vote_candidate(4);
				break;
			case LD_CANDIDATES:
				/*protocol: reads the list of candidates from server */
				recv(sock, client_candidates, N_CANDIDATES * sizeof(vote_t), 0);
				break;
			case HLT_BALLOT:
				/* protocol: halts the ballot and send the amount of votes */
				send(sock, client_candidates, N_CANDIDATES * sizeof(vote_t), 0);
				close(sock);
				fflush(stdout);
				free(servername);
				free(client_candidates);
				exit(EXIT_SUCCESS);
				break;
			default:
				fprintf(stderr, "bad opcode. try again\n");
				goto read_opcode;
		};
	}
}
