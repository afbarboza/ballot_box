#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#define	JSON_LEN	1024

#define	raise_invalid_parse()	fprintf(stderr, "error: invalid parse at %s:%d\n", __FILE__, __LINE__); \
				exit(EXIT_FAILURE)

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
	vote_t retval;

	if (json_str == NULL) {
		fprintf(stderr, "error: null pointer at %s:%d\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* retrieves the pair field:value from json string */
	cJSON *root = cJSON_Parse(json_str);
	cJSON *json_code 	= cJSON_GetObjectItemCaseSensitive(root, "code");
	cJSON *json_candidate	= cJSON_GetObjectItemCaseSensitive(root, "name");
	cJSON *json_party	= cJSON_GetObjectItemCaseSensitive(root, "party");
	cJSON *json_num_votes	= cJSON_GetObjectItemCaseSensitive(root, "num_votes");

	/* check sanity of integer values */
	if (!cJSON_IsNumber(json_code) || !cJSON_IsNumber(json_num_votes)) {
		raise_invalid_parse();
	}

	/* check sanity of string values */
	if (!cJSON_IsString(json_candidate) || !cJSON_IsString(json_party)) {
		raise_invalid_parse();
	}

	/* fill the struct vote */
	retval.n_votes 		= (int) json_num_votes->valuedouble;
	retval.vote_code 	= (int) json_code->valuedouble;
	strcpy(retval.candidate_party, json_party->valuestring);
	strcpy(retval.candidate_name, json_candidate->valuestring);

	return retval;
}

void write_json_file(const char *filename, char **json_strings, int n_strings)
{
	int i;
	FILE *json_fd = NULL;

	/* creates the json file storing array of objects */
	json_fd = fopen(filename, "w+");

	/* walks a lis of json strings and write to the disk */
	for (i = 0; i < n_strings; i++) {
		if (i == 0) {
			/* if SEEK_SET, then adds the '[' */
			fprintf(json_fd, "[%s,", json_strings[i]);
		} else if (i == (n_strings - 1)) {
			/* if SEEK_END, then adds the ']' */
			fprintf(json_fd, "%s]", json_strings[i]);
		} else {
			/* otherwise, adds the ',' character */
			fprintf(json_fd, "%s,", json_strings[i]);
		}
	}

	fclose(json_fd);
}

char **extract_json_list(const char *jsonstr, int *length)
{
	int i = 0;
	int json_len = 0;
	int list_len = 0;
	char *tmp = NULL;
	char *token = NULL;
	char **json_list = NULL;

	if (jsonstr == NULL) {
		fprintf(stderr, "error: null pointer at %s:%d\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* creates a copy of @jsonstr, once it will be altered inside this function */
	json_len = strlen(jsonstr);
	tmp = (char *) malloc(json_len * sizeof(char));
	strcpy(tmp, jsonstr);

	/* alloc memory for the list */
	json_list = (char **) realloc(json_list, (list_len + 1) * sizeof(char *));

	/* breaking the @jsonstr into a sequence of json strings */
	token = strtok(tmp, ",[]");
	while (token != NULL) {
		json_list[list_len] = token;
		#ifdef	DEBUG
		printf("__%s__", json_list[list_len]);
		#endif
		token = strtok(NULL, ",[]");
		list_len++;
		json_list = (char **) realloc(json_list, (list_len + 1) * sizeof(char *));
	}

	*length = list_len;
	free(tmp);
	return json_list;
}
