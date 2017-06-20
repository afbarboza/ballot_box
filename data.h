#ifndef	_DATA_H_
#define	_DATA_H_

#define	N_CANDIDATES	5
#define	MAX_STRLEN	28

#include <pthread.h>

struct vote {
	int	n_votes;
	int	vote_code;
	char	candidate_party[MAX_STRLEN];
	char	candidate_name[MAX_STRLEN];
} __attribute__((aligned(32)));

typedef struct vote vote_t;

vote_t	candidates[N_CANDIDATES];

extern pthread_mutex_t	lock_candidate;

void load_candidates(void);

void vote_candidate(int code);

#endif	/* data.h */
