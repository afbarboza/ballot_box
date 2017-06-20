#include "data.h"

#include <string.h>

pthread_mutex_t lock_candidate;

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

void load_candidates(void)
{
	int i, j;

	/* initializes the candidate and party names */
	for (i = 0; i < N_CANDIDATES; i++) {
		for (j = 0; j < MAX_STRLEN; j++) {
			candidates[i].candidate_party[j] = ' ';
			candidates[i].candidate_name[j] = ' ';
		}
	}

	/* initializes informartions about votes */
	candidates[0].n_votes = 0;
	candidates[0].vote_code = 13;
	strcpy(candidates[0].candidate_party, "PT");
	strcpy(candidates[0].candidate_name, "Lula");

	candidates[1].n_votes = 0;
	candidates[1].vote_code = 45;
	strcpy(candidates[1].candidate_party, "PSDB");
	strcpy(candidates[1].candidate_name, "Aecio");

	candidates[2].n_votes = 0;
	candidates[2].vote_code = 25;
	strcpy(candidates[2].candidate_party, "DEM");
	strcpy(candidates[2].candidate_name, "Temer");


	candidates[3].n_votes = 0;
	candidates[3].vote_code = 0;
	strcpy(candidates[3].candidate_party, "branco");
	strcpy(candidates[3].candidate_name, "branco");

	candidates[4].n_votes = 0;
	candidates[4].vote_code = 1;
	strcpy(candidates[4].candidate_party, "nulo");
	strcpy(candidates[4].candidate_name, "nulo");

}

void vote_candidate(int code)
{
	pthread_mutex_lock(&lock_candidate);
	candidates[code].n_votes++;
	pthread_mutex_unlock(&lock_candidate);
}
