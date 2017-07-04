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

	vote_t v3 = {
		.n_votes = 10,
		.vote_code = 50,
		.candidate_party = "PSOL",
		.candidate_name = "Luciana"
	};


	vote_t v4 = {
		.n_votes = 11,
		.vote_code = 25,
		.candidate_party = "PMDB",
		.candidate_name = "Temer"
	};

	char *json_votes[4];

	json_votes[0] = build_json(&v);
	json_votes[1] = build_json(&v2);
	json_votes[2] = build_json(&v3);
	json_votes[3] = build_json(&v4);

	printf("%s\n%s\n%s\n%s\n", json_votes[0], json_votes[1], json_votes[2], json_votes[3]);


	vote_t v5 = parse_json(json_votes[0]);
	vote_t v6 = parse_json(json_votes[1]);


	puts_vote(&v5);
	puts_vote(&v6);

	write_json_file("test.json", json_votes, 4);

	char *txt_json = stringify_json("test.json");
	printf("\n\n\njson txt: \n\n\n%s\n\n", txt_json);

	int i;
	int size_list = 0;
	char **json_tokens = extract_json_list(txt_json, &size_list);
	
	for (i = 0; i < size_list; i++) {
		printf("%s\n", json_tokens[i]);
	}

	return 0;
}
