#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "json.h"

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

	return 0;
}
