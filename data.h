#ifndef	_DATA_H_
#define	_DATA_H_

#define	N_CANDIDATES	5
#define	MAX_STRLEN	28

#include <pthread.h>

#define	CLIENT_CODE
#define	SERVER_CODE

struct vote {
	int	n_votes;
	int	vote_code;
	char	candidate_party[MAX_STRLEN];
	char	candidate_name[MAX_STRLEN];
} __attribute__((aligned(32)));


enum opcode {
	VOTE 		= 1,
	VOTE_BLANK 	= 2,
	VOTE_NULL 	= 3,
	LD_CANDIDATES	= 999,
	HLT_BALLOT 	= 888,
	PANIC		= 666
};

typedef struct vote vote_t;

vote_t	server_candidates[N_CANDIDATES];
vote_t  *client_candidates;

extern pthread_mutex_t	lock_candidate;

SERVER_CODE
void load_candidates(void);

SERVER_CODE
void count_amount_votes(vote_t *server_candidates, vote_t *client_votes);


SERVER_CODE
void rank_server_candidates(void);

/**
 * check_sanity_vote - checks if a @vote is a valid vote
 */
CLIENT_CODE
int check_sanity_vote(int code);



/**
 * output_main_menu -  	shows the main options to the client:
 *			(1) vote,
 *			(2) vote in blank,
 *			(3) vote null,
 *			(999) load the candidates list from server
 *			(888) send the ballots votes
 */
CLIENT_CODE
void output_main_menu(void);



/**
 * output_candidates - 	shows the @list of valid candidates
 *			(previously read from server)
 *
 *	@list: the list of candidates incoming from server
 */
CLIENT_CODE
void output_candidates(vote_t *list);



/**
 * get_user_vote - reads the vote from user for a candidates
 *
 */
CLIENT_CODE
void get_user_vote(void);



/**
 * inc_vote_candidate - increments the amount of votes by one
 *			to a given candidates
 *
 *	@idx: the index of the candidate at candidate table
 *
 */
CLIENT_CODE
void inc_vote_candidate(int idx);

#endif	/* data.h */
