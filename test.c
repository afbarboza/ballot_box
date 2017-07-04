#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "json.h"

void puts_vote(vote_t *v)
{
	printf("n_vote: %d\n", 		v->n_votes);
	printf("vote_code: %d\n", 	v->vote_code);
	printf("name: %s\n", 		v->candidate_name);
	printf("party: %s\n", 		v->candidate_party);
	puts("\n---*---\n");
}

int main(void)
{
	vote_t v = {
		.n_votes = 20,
		.vote_code = 13,
		.candidate_party = "PT",
		.candidate_name = "Lula"
	};

	vote_t v2 = {
		.n_votes = 5,
		.vote_code = 45,
		.candidate_party = "PSDB",
		.candidate_name = "Aecio"
	};

	char *json_vote = build_json(&v);
	char *json_vote2 = build_json(&v2);

	printf("%s\n%s\n", json_vote, json_vote2);


	vote_t v3 = parse_json(json_vote);
	vote_t v4 = parse_json(json_vote2);

	puts_vote(&v3);
	puts_vote(&v4);

	return 0;
}
