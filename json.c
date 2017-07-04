#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#define	JSON_LEN	1024

char *stringify_json(const char *json_filename)
{
	int fsize = 0;
	FILE *fd = NULL;
	char *buffer = NULL;

	/* check sanity of filename */
	if (json_filename == NULL) {
		fprintf(stderr, "invalid filename");
		exit(EXIT_FAILURE);
	}

	/* check whether the file does exist */
	fd = fopen(json_filename, "r");
	if (fd == NULL) {
		fprintf(stderr, "file does not exist");
		exit(EXIT_FAILURE);
	}

	/* reads the file from disk */
	while (!feof(fd)) {
		buffer = (char *) realloc(buffer, ((fsize + 1) * sizeof(char)));
		fscanf(fd, "%c", &(buffer[fsize]));
		fsize++;
	}

	return buffer;
}

char *build_json(vote_t *src)
{
	char *json_vote = NULL;

	if (src == NULL) {
		fprintf(stderr, "null pointer at %s:%d\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	json_vote = (char *) malloc(JSON_LEN * sizeof(char));
	sprintf	(json_vote, 
		"{\"code\": %d, \"name\": \"%s\", \"party\": \"%s\", \"num_votes\":%d}",
		src->vote_code, src->candidate_name, src->candidate_party, src->n_votes);
	return json_vote;
}

vote_t parse_json(const char *json_str)
{
	/* TODO: code here */
}
