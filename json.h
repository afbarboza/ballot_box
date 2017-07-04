#ifndef	_JSON_H_
#define	_JSON_H_

#include "data.h"

char *stringify_json(const char *json_filename);
char *build_json(vote_t *src);

#endif	/* json.h */
