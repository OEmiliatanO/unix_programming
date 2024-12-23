/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a,const void *b) {
    return strcmp((char *)a,((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec * table;
	static int numrec;
	Dictrec * found = (Dictrec *)-1;
	static int first_time = 1;
    long filsiz;

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;

		first_time = 0;

		/* Open the dictionary.
		 * Fill in code. */
        fd = open(resource, O_RDONLY);
        if (fd == -1) {
            DIE("open");
        }

		/* Get record count for building the tree. */
		filsiz = lseek(fd,0L,SEEK_END);
        if (filsiz < 0) {
            DIE("lseek");
        }
		numrec = filsiz / sizeof(Dictrec);

		/* mmap the data.
		 * Fill in code. */
        table = (Dictrec *)mmap(NULL, filsiz, PROT_READ, MAP_SHARED, fd, 0);
        if (table == MAP_FAILED) {
            DIE("mmap");
        }
		close(fd);
	}
    
	/* search table using bsearch
	 * Fill in code. */
    int l = 0, r = numrec;
    while (r - l > 0) {
        int mid = (l + r) >> 1;
        int cmp = dict_cmp(sought->word, table[mid].word);
        if (cmp == 0) {
            found = table + mid;
            break;
        } else if (cmp < 0) {
            r = mid;
        } else if (cmp > 0) {
            l = mid + 1;
        }
    }

	if (found != (Dictrec *)-1) {
        memset(sought->text, 0, sizeof(sought->text));
		strcpy(sought->text,found->text);
		return FOUND;
	}

	return NOTFOUND;
}
