/*
 * lookup6 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by shared memory
 *
 * Synchronization is by two semaphores:
 *   One excludes other clients while we talk to server
 *   The other is used BOTH to alert the server and to await his reply
 *   Client raises a semaphore by 2. Server drops by 1 before and
 *   after doing lookup
 * The same key (which is passed as resource) is used BOTH 
 * for shared memory & semaphores.
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int shmfd;
    static sem_t *semfd[2];
	long key = strtol(resource,(char **)NULL,16);
	static Dictrec * shm;
	static int first_time = 1;

	if (first_time) {        /* attach to shared memory & semaphores */
		first_time = 0;

		/* Connect to shared memory.
		 * Fill in code. */
        char shm_path[MAX_FILENAME];
        snprintf(shm_path, MAX_FILENAME, SHM_FILE, key);
        shmfd = shm_open(shm_path, O_CREAT | O_RDWR, 0666);
        if (shmfd == -1) {
            DIE("shm_open");
        }
        if (ftruncate(shmfd, sizeof(Dictrec)) == -1) {
            DIE("ftruncate");
        }

		/* Get shared memory virtual address.
		 * Fill in code. */
        shm = (Dictrec *)mmap(NULL, sizeof(Dictrec), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        if (shm == MAP_FAILED) {
            DIE("mmap");
        }

		/* Get semaphore.
		 * Fill in code. */
        char sem_path[2][MAX_FILENAME];
        snprintf(sem_path[0], MAX_FILENAME, SEM_FILE, 0, key); // wait for client to increment
        snprintf(sem_path[1], MAX_FILENAME, SEM_FILE, 1, key); // for sync with client
        semfd[0] = sem_open(sem_path[0], O_CREAT | O_RDWR, 0666);
        semfd[1] = sem_open(sem_path[1], O_CREAT | O_RDWR, 0666);
        if (semfd[0] == SEM_FAILED || semfd[1] == SEM_FAILED) {
            DIE("sem_open");
        }
	}

	/* Reserve semaphore so other clients will wait.
	 * Fill in code. */
    sem_wait(semfd[1]);
    //fprintf(stderr, "strcpy\n");
	strcpy(shm->word,sought->word);
    //fprintf(stderr, "strcpy completed\n");

	/* Alert server.  Bump semaphore 0 up.
	 * Fill in code. */
    sem_post(semfd[0]);

	/* Wait for server to finish.  Server will have set sem 0 to zero.
	 * Fill in code. */
    sem_wait(semfd[1]);

	/* Done using the server.  Release to other clients.
	 * Fill in code. */

	if (strcmp(shm->text,"XXXX") != 0) {
		strcpy(sought->text,shm->text);
        sem_post(semfd[0]);
		return FOUND;
	}

    sem_post(semfd[0]);
	return NOTFOUND;
}
