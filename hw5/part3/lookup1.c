/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <string.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	Dictrec dr;
	(void) dr;
	static FILE * in;
	static int first_time = 1;

	if (first_time) { 
		first_time = 0;
		/* open up the file
		 *
		 * Fill in code. */
		in = fopen(resource, "r");
	}

	/* read from top of file, looking for match
	 *
	 * Fill in code. */
	rewind(in);
	char word[WORD], text[TEXT];
	while (!feof(in)) {
		/* Fill in code. */
		fread(word, sizeof(char), WORD, in); fgetc(in); 
		fread(text, sizeof(char), TEXT, in); fgetc(in); fgetc(in);
		if (strcmp(sought->word, word) == 0) {
			strcpy(sought->text, text);
			return FOUND;
		}
	}

	return NOTFOUND;
}
