/*
 * shm_server : wait on a semaphore; do lookup ; use shared
 *              memory for communication ; notify using same 
 *              semaphore (client raises by 2; server lowers
 *              by 1 both to wait and to notify)
 *
 *              argv[1] is the name of the local file
 *              argv[2] is the key for semaphores + shared mem
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>

#include "dict.h"

char sem_path[2][MAX_FILENAME];
char shm_path[MAX_FILENAME];
void cleanup() {
    sem_unlink(sem_path[0]);
    sem_unlink(sem_path[1]);
    unlink(shm_path);
}

int main(int argc, char **argv) {
	Dictrec * shm;
	struct stat stbuff;
	extern int errno;

	if (argc != 3) {
		fprintf(stderr,"Usage : %s <dictionary source> <Resource/Key>\n",argv[0]);
		exit(errno);
	}

    char *endptr;
    long key = strtol(argv[2], &endptr, 16);

	/* Create & initialize shared memory & semaphores */
    snprintf(shm_path, MAX_FILENAME, SHM_FILE, key);
    int shmfd = shm_open(shm_path, O_CREAT | O_RDWR, 0666);
    if (shmfd == -1) {
        DIE("shm_open");
    }
    if (ftruncate(shmfd, sizeof(Dictrec)) == -1) {
        DIE("ftruncate");
    }

    snprintf(sem_path[0], MAX_FILENAME, SEM_FILE, 0, key); // notify server to lookup & lookup is done
    snprintf(sem_path[1], MAX_FILENAME, SEM_FILE, 1, key); // notify server is available
    sem_t *semfd[] = {sem_open(sem_path[0], O_CREAT | O_RDWR, 0666), sem_open(sem_path[1], O_CREAT | O_RDWR, 0666)};
    if (semfd[0] == SEM_FAILED) {
        DIE("sem_open 0");
    }
    if (semfd[1] == SEM_FAILED) {
        DIE("sem_open 1");
    }
    fprintf(stderr, "%s\n", sem_path[0]);
    fprintf(stderr, "%s\n", sem_path[1]);
    if (atexit(cleanup) == -1) {
        DIE("atexit");
    }
    sem_init(semfd[0], 1, 0);
    sem_init(semfd[1], 1, 0);

	/* Verify database resource is present. */
	if (stat(argv[1],&stbuff) == -1) {
		DIE(argv[1]);
    }

	/* Get shared memory virtual address.
	 * Fill in code. */
    shm = (Dictrec *)mmap(NULL, sizeof(Dictrec), PROT_WRITE | PROT_READ, MAP_SHARED, shmfd, 0);
    if (shm == MAP_FAILED) {
        DIE("mmap");
    }
    memset(shm, 0, sizeof(Dictrec));

	/* Main working loop */
	for (;;) {

		/* When we are first started, value is zero.  Client sets to two to wake us up.
		 * Try to decrement sem 0.  
		 * Then we will wait here until the semaphore is non-zero
		 *
		 * Fill in code. */
        sem_post(semfd[1]); // Server is available
        sem_wait(semfd[0]);

		/* Do the lookup here.  Write result directly into shared memory. */
		switch(lookup(shm,argv[1]) ) {
			case FOUND: 
				break;
			case NOTFOUND: 
				strcpy(shm->text,"XXXX");
				break;
			case UNAVAIL:
				DIE(argv[1]);
		}
			
		/* Decrement again so that we will block at the top of the for loop again until a client wakes us up again.
		 *
		 * Fill in code. */
        sem_post(semfd[1]);
        sem_wait(semfd[0]); // wait for client to copy

	} /* end for */
}
