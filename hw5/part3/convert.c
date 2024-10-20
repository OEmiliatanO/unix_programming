/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BUF_SIZE 200
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;        /* defaults */
	static Dictrec dr;
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if (argc==3) {
		if ((in =fopen(argv[1],"r")) == NULL){DIE(argv[1]);}
		if ((out =fopen(argv[2],"w")) == NULL){DIE(argv[2]);}	
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	/* Main reading loop : read word first, then definition into dr */

	/* Loop through the whole file. */
	while (!feof(in)) {
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */
		char word[WORD], text[TEXT], buf[BUF_SIZE];
		memset(word, 0, sizeof(word));
		memset(text, 0, sizeof(text));
		
		fgets(word, WORD, in);
		int word_len = strlen(word);
		int text_len = 0;
		word[word_len-1] = '\0';
		while (text_len < TEXT && fgets(buf, BUF_SIZE, in)) {
			int len = strlen(buf);
			if (len > 0 && buf[len-1] == '\n') {
				buf[len-1] = '\0';
				--len;
			}
			
			if (text_len + len + 1 >= TEXT) {
				buf[TEXT - text_len] = '\0';
			}
			if (strcat(text, buf) == NULL) {
				perror("Cannot concatenate the string.");
				return EXIT_FAILURE;
			}
			text_len += len;

			if (len == 0) {
				break; // blank line
			}
		}
		

		/* Read word and put in record.  Truncate at the end of the "word" field.
		 *
		 * Fill in code. */
		strcpy(dr.word, word);
		

		/* Read definition, line by line, and put in record.
		 *
		 * Fill in code. */
		strcpy(dr.text, text);

		/* Write record out to file.
		 *
		 * Fill in code. */
		fwrite(word, sizeof(char), WORD, out); fputs("\n", out);
		fwrite(text, sizeof(char), TEXT, out); fputs("\n\n", out);
	}

	fclose(in);
	fclose(out);
	return 0;
}
