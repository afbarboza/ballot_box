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
#include "json.h"

#define MAXLEN			0xFF
#define	MAX_PENDING_CONNECTIONS	10
#define	SERVER_PORT		40012

/**
 * handle_error - 	handles unhandable errors in the TCP protocol
 *			and halts the program
 */
__attribute__((noinline, noreturn))
void handle_error(void)
{
	char *errmsg = NULL;
	errmsg = strerror(errno);
	perror(errmsg);
	exit(EXIT_FAILURE);
}

/**
 *
 * send_vote_files - 	send the json files of votes to the client.
 *
 *
 * @socket: 	the opened socket, used for comunication to the client.
 *		(must be initialized and opened before this function be called)
 *
 * @str_file:	pointer to the file to be sent to the client.
 *
 */
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

/**
 * recv_vote_files - 	receives a json file of votes, incoming from the client.
 *
 *
 * @socket:	the opened socket, used for comunication to the client.
 *		(must be initialized and opened before this function be called)
 *
 *
 * @return:	the string containing the contents of json file.
 *		(this string is null-terminated)
 *
 */
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

/**
 * puts_vote - print out a vote
 */
void puts_vote(vote_t *v)
{
	printf("n_vote: %d --- ", v->n_votes);
	printf("vote_code: %d --- ", v->vote_code);
	printf("name: %s --- ", v->candidate_name);
	printf("party: %s \n\n", v->candidate_party);
}

/**
 * read_opcode - reads and returns an opcode incoming from client.
 *
 * @socket:	socket used to receive the opcode
 */
int read_opcode(int socket)
{
	char c;
	int i = 0, op;
	char *tmpstr = NULL;

	/* bufferize the string storing the opcode */
	do {
		tmpstr = (char *) realloc(tmpstr, (i + 1) * sizeof(char));
		recv(socket, &c, sizeof(char), 0);
		tmpstr[i] = c;
		i++;
	} while (c != '\n');
	tmpstr[i - 1] = '\0';

	/* converts it to an integer */
	op = atoi(tmpstr);
	free(tmpstr);
	return op;
}

/**
 * parse_incoming_votes - reads a json-string of votes and 
 *			conerts into a vote_t buffer.
 *
 * @str_client_votes: the json-string of votes
 *
 * @return: the buffer of votes computed by the client
 */
vote_t *parse_incoming_votes(char *str_client_votes)
{
	int len = 0, i;
	vote_t *buffer;
	if (str_client_votes == NULL) {
		fprintf(stderr, "error: null pointer at %s:%d\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* tokenize the json file of votes incoming from client */
	char **incoming_votes = extract_json_list(str_client_votes, &len);

	/* temporary buffer of files used to compute the partial results of elections */
	buffer = (vote_t *) malloc(len * sizeof(vote_t));


	#ifdef	DEBUG
		printf("%s:%s - %d parsed votes: \n", __FILE__, __func__, len);
	#endif

	/* converts the list of tokens into a buffer ov votes vote_t */
	for (i = 0; i < len; i++) {
		buffer[i] = parse_json(incoming_votes[i]);
		#ifdef	DEBUG
			puts_vote(&buffer[i]);
		#endif
	}

	return buffer;
}

/**
 * halt_client_connection - ends a client connection
 *
 * @sock: the socket descriptor of client connection
 * @buffer_ballot: the buffer of votes readed from client
 *
 *
 */
__attribute__((noinline))
void *halt_client_connection(int sock, vote_t *buffer_ballot)
{
	/* shows the partial results of the ellections */
	rank_server_candidates();

	/* free the used buffer of votes */
	if (buffer_ballot != NULL)
		free(buffer_ballot);

	/* log-keeping warning the end of connection*/
	printf("closing down connection id: %d\n", sock);

	/* kill the thread of connection between server and client */
	pthread_exit(EXIT_SUCCESS);
	return NULL;
}

/**
 * init_msg_xchg - initializes and maintains the message exchange
 *		server and client. This is the body of the main
 *		pthread function.
 */
void *init_msg_xchg(void *socket_descriptor)
{
	int client_opcode = 0;
	int connected_sock = 0;
	connected_sock = (int) socket_descriptor;
	char *str_incoming_votes = NULL;

	/* stores the incoming ballot from client */
	vote_t *buffer_ballot = NULL;

	/* warns about a new connecion thread */
	printf("new connection id: %d\n", connected_sock);

	/* keep up the protocol exchange between server and client */
	while (1) {
		/* protocol: read the command opcode from client */
		client_opcode = read_opcode(connected_sock);

		switch (client_opcode) {
			case LD_CANDIDATES:
				/* protocol: send the list of candidates */
				send_vote_files(connected_sock, file_server_candidates);
				break;

			case HLT_BALLOT:
				/* protocol: read the votes coming from the client, count it and halt connection */
				str_incoming_votes = recv_vote_files(connected_sock);
				buffer_ballot = parse_incoming_votes(str_incoming_votes);
				count_amount_votes(server_candidates, buffer_ballot);
				free(str_incoming_votes);
				return halt_client_connection(connected_sock, buffer_ballot);
			default:
				/* protocol: we cannot handle bad opcoded */
				fprintf(stderr, "panc: bad opcode");
				return halt_client_connection(connected_sock, buffer_ballot);
		};
	}
}

/**
 * init_candidates_file - intialize the file(i.e. string ) of candidates 
 *			sent to the client
 *
 */
void init_candidates_file(void)
{
	int tmplen = 0, i;
	char *tmp_file_candidate = NULL;
	file_server_candidates = NULL;

	for (i = 0; i < N_CANDIDATES; i++) {
		/* parse each struct vote_t to a json string */
		tmp_file_candidate = build_json(&server_candidates[i]);

		/* gets more room to the file to be sent */
		tmplen += strlen(tmp_file_candidate);
		file_server_candidates = (char *) realloc(file_server_candidates, (tmplen + 6) * sizeof(char));

		if (tmp_file_candidate == NULL) {
			fprintf(stderr, "error: null pointer at %s:%s:%d\n", __FILE__, __func__, __LINE__);
			exit(EXIT_FAILURE);
		}

		if (i == 0) {
			strncat(file_server_candidates, "[", 1);
			strcat(file_server_candidates, tmp_file_candidate);
			strncat(file_server_candidates, ",", 1);
		} else if (i == (N_CANDIDATES - 1)) {
			strcat(file_server_candidates, tmp_file_candidate);
			strncat(file_server_candidates, "]\n\0", 3);
		} else {
			strcat(file_server_candidates, tmp_file_candidate);
			strncat(file_server_candidates, ",", 1);
		}
	}
}

int main(void)
{
	char flag = 1;
	int sock, i = 0;
	struct sockaddr_in client_addr;


	/* load list of all candidates in server */
	load_candidates();

	/* intializes the buffer file of candidates at server side */
	init_candidates_file();

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
		pthread_create(server_thread, NULL, init_msg_xchg, (void *) connected);

		fflush(stdout);
	}

	close(sock);
	exit(EXIT_SUCCESS);
}
