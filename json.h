#ifndef	_JSON_H_
#define	_JSON_H_

#include "data.h"
#include <stdio.h>


/**
 * stringify_json - reads a .json file and bufferizes it in a string
 *
 * @json_filename: the filename which will be opened.
 */
char *stringify_json(const char *json_filename);

/**
 * build_json - reads a vote_t and converts it in a string
 *		in json format.
 * 
 * @src: the string to be converted to json
 */
char *build_json(vote_t *src);

/**
 * parse_json - reads a json string an converts it to
 *		a vote_t datatype.
 *
 * @json_str: the string to be converted to a vote_t datatype
 */
vote_t parse_json(const char *json_str);

/**
 * write_json_file - writes to the disk a set of json strings
 *
 * @filename: 	the file with .json to be created
 *		(if already exists, it will be truncated)
 *
 * @json_strings: list of json strings to be write to the disk.
 *
 * @n_strings: number of strings to be written.
 *
 *
 */
void write_json_file(const char *filename, char **json_strings, int n_strings);

/**
 * extract_json_list: 	receives a single string, representing an array 
 *			of json objects and converts into an array of
 *			strings, where each string represents a single
 *			object in json format.
 *
 * @jsonstr:	the string storing the array of json objects.
 *
 * @length:	the length of the list returned by this function.
 *		(it muts be initialized before this function is called)
 *
 * @return:	an array of strings, where each string represents a
 *		single json object.
 */
char **extract_json_list(const char *jsonstr, int *length);

#endif	/* json.h */
