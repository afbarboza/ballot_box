#include "data.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

pthread_mutex_t lock_candidate;

SERVER_CODE
void load_candidates(void)
{
	int i, j;

	/* initialize the mutex of votes at server side */
	pthread_mutex_init(&lock_candidate, NULL);

	/* initializes the candidate and party names */
	for (i = 0; i < N_CANDIDATES; i++) {
		for (j = 0; j < MAX_STRLEN; j++) {
			server_candidates[i].candidate_party[j] = ' ';
			server_candidates[i].candidate_name[j] = ' ';
		}
	}

	/* initializes informartions about votes */
	server_candidates[0].n_votes = 0;
	server_candidates[0].vote_code = 13;
	strcpy(server_candidates[0].candidate_party, "PT");
	strcpy(server_candidates[0].candidate_name, "Lula");

	server_candidates[1].n_votes = 0;
	server_candidates[1].vote_code = 45;
	strcpy(server_candidates[1].candidate_party, "PSDB");
	strcpy(server_candidates[1].candidate_name, "Aecio");

	server_candidates[2].n_votes = 0;
	server_candidates[2].vote_code = 25;
	strcpy(server_candidates[2].candidate_party, "DEM");
	strcpy(server_candidates[2].candidate_name, "Temer");

	server_candidates[3].n_votes = 0;
	server_candidates[3].vote_code = 0;
	strcpy(server_candidates[3].candidate_party, "branco");
	strcpy(server_candidates[3].candidate_name, "branco");

	server_candidates[4].n_votes = 0;
	server_candidates[4].vote_code = 1;
	strcpy(server_candidates[4].candidate_party, "nulo");
	strcpy(server_candidates[4].candidate_name, "nulo");

}

SERVER_CODE
void count_amount_votes(vote_t *server_candidates, vote_t *client_votes)
{
	int i;

	if (server_candidates == NULL || client_votes == NULL) {
		fprintf(stderr, "error: null pointer at %s:%d", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < N_CANDIDATES; i++) {
		pthread_mutex_lock(&lock_candidate);

		server_candidates[i].n_votes += client_votes[i].n_votes;
		pthread_mutex_unlock(&lock_candidate);
	}
}

SERVER_CODE
void rank_server_candidates(void)
{
	int i;

	puts("\n\n");
	for (i = 0; i < N_CANDIDATES; i++) {
		pthread_mutex_lock(&lock_candidate);
		printf("candidato %s tem %d votos\n", 	server_candidates[i].candidate_name,
							server_candidates[i].n_votes);
		pthread_mutex_unlock(&lock_candidate);
	}
	puts("\n\n");
}

CLIENT_CODE
static int map_code_party(int vote_code)
{
	switch(vote_code) {
		case 13:
			return 0;
		case 45:
			return 1;
		case 25:
			return 2;
		case 0:
			return 3;
		case 1:
			return 4;
		default:
			return -1;
	};
}

CLIENT_CODE
int check_sanity_vote(int code)
{
	return ((map_code_party(code) == -1) ? 0 : 1);
}


CLIENT_CODE
void inc_vote_candidate(int idx)
{
	client_candidates[idx].n_votes++;
}

CLIENT_CODE
void output_main_menu(void)
{
        puts("\n---*--- MENU ---*---\n");
        puts("1\t\t- votar");
        puts("2\t\t- votar branco");
        puts("3\t\t- votar nulo");
        puts("999\t\t- carregar candidatos");
        puts("888\t\t- finalizar votacoes");
}

CLIENT_CODE
void output_candidates(vote_t *list)
{
	int i;

	if (list == NULL) {
		fprintf(stderr, "error: null pointer at %s:%d", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < (N_CANDIDATES - 2); i++) {
		printf("%d: %s(%s)\n",	list[i].vote_code,
					list[i].candidate_name,
					list[i].candidate_party);
	}
}

CLIENT_CODE
void get_user_vote(void)
{
	int code_party_usr = 0, idx = 0;

	/* clean the screen */
	system("clear");

	/* show the list of possible candidates */
	output_candidates(client_candidates);

read_vote:
	/* reads the party code */
	scanf("%d", &code_party_usr);

	/* check for wrong codes */
	if (!check_sanity_vote(code_party_usr)) {
		fprintf(stderr, "invalid vote code. try again.");
		goto read_vote;
	}

	/* maps the vote to the actual index at table of candidates */
	idx = map_code_party(code_party_usr);

	/* increments the number of votes to the candidate */
	inc_vote_candidate(idx);
}
