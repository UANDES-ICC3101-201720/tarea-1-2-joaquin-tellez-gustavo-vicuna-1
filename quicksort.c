#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include "types.h"
#include "const.h"
#include "util.h"

pthread_t pt[6];

struct final
{
    UINT* Z;
    int lo;
    int hi;
};

int dep = 1;

// TODO: implement
void swap(UINT* A, int in, int fin){
	int temp = A[in];
	A[in] = A[fin];
	A[fin] = temp; 
}

int partition(UINT* A, int lo, int hi) {
	int pivot = A[hi];
	int i = lo;
	for (int j = lo; j < hi; j++) {
		if (A[j] < pivot) {
			swap(A, i, j);
			i = i + 1;
		}
	}
	swap(A, i, hi);
	return i;
}

int quicksort(UINT* A, int lo, int hi) {
    if (lo < hi) {
	int p = partition(A, lo, hi);
	quicksort(A, lo, p - 1 );
	quicksort(A, p + 1, hi);
    }
    return 0;
}

void parallel_quicksort(UINT* A, int lo, int hi);

void *first(void * fv){
	struct final *fs = fv;
    	parallel_quicksort(fs->Z, fs->lo, fs->hi);
    	return NULL;
}

// TODO: implement
void parallel_quicksort(UINT* A, int lo, int hi) {
	int pivot = partition(A, lo, hi);
	
	if (dep-- > 0){
		/*struct final *fi = malloc(sizeof(struct final)+(h)*sizeof(UINT*));
		fi->depth = dep;
		fi->lo = l;
		fi->hi = h;
		memcpy(fi->Z, A, (h)*sizeof(UINT*));*/
		struct final fi = {A, lo, hi};;
		pthread_t first_thread;
		pthread_create(&first_thread, NULL, first, &fi);
		//parallel_quicksort(A, pivot+1, h);
		pthread_join(first_thread, NULL);
        pthread_t second_thread;
		pthread_create(&second_thread, NULL, first, &fi);
		pthread_join(second_thread, NULL);
        pthread_t third_thread;
		pthread_create(&third_thread, NULL, first, &fi);
		pthread_join(third_thread, NULL);
        pthread_t forth_thread;
		pthread_create(&forth_thread, NULL, first, &fi);
		pthread_join(forth_thread, NULL);
        pthread_t fifth_thread;
		pthread_create(&fifth_thread, NULL, first, &fi);
		pthread_join(fifth_thread, NULL);
        pthread_t sixth_thread;
		pthread_create(&sixth_thread, NULL, first, &fi);
		pthread_join(sixth_thread, NULL);
	}
	else{
		quicksort(A, lo, pivot-1);
		quicksort(A, pivot+1, hi);
	}
}

int main(int argc, char** argv) {
    printf("[quicksort] Starting up...\n");

    /* Get the number of CPU cores available */
    printf("[quicksort] Number of cores available: '%ld'\n",
           sysconf(_SC_NPROCESSORS_ONLN));

    /* TODO: parse arguments with getopt */
    char* Tvalue;
	int Evalue = 0;
	int index;
	int c;

	opterr = 0;


	while ((c = getopt (argc, argv, "E:T:")) != -1)
		switch (c){
			case 'E':
				Evalue = atoi(optarg);
				break;
			case 'T':
				Tvalue = optarg;
				break;
			case '?':
				if (optopt == 'T' || optopt == 'E')
				  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
				  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
				  fprintf (stderr,
					   "Unknown option character `\\x%x'.\n",
					   optopt);
				return 1;
			default:
				abort ();
		}
	for (index = optind; index < argc; index++)
		printf ("Non-option argument %s\n", argv[index]);
    printf("\n[quicksort] E:%i , %s\n",Evalue,Tvalue);

    /* TODO: start datagen here as a child process. */

    pid_t pid;
	pid = fork();
	if (pid == -1){   
		fprintf(stderr, "Error en fork\n");
		exit(EXIT_FAILURE);
	}
	if (pid == 0){
		execlp("./datagen","./datagen",NULL);
	}

    /* Create the domain socket to talk to datagen. */
    struct sockaddr_un addr;
    int fd;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("[quicksort] Socket error.\n");
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, DSOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("[quicksort] connect error.\n");
        close(fd);
        exit(-1);
    }

    /* DEMO: request two sets of unsorted random numbers to datagen */
    for (int i = 0; i < Evalue; i++) {
        /* T value 3 hardcoded just for testing. */
        char begin[10];
        strcpy(begin,"BEGIN U");
	    strcat(begin, Tvalue);

        int rc = strlen(begin);

        /* Request the random number stream to datagen */
        if (write(fd, begin, strlen(begin)) != rc) {
            if (rc > 0) fprintf(stderr, "[quicksort] partial write.\n");
            else {
                perror("[quicksort] write error.\n");
                exit(-1);
            }
        }

        /* validate the response */
        char respbuf[10];
        read(fd, respbuf, strlen(DATAGEN_OK_RESPONSE));
        respbuf[strlen(DATAGEN_OK_RESPONSE)] = '\0';

        if (strcmp(respbuf, DATAGEN_OK_RESPONSE)) {
            perror("[quicksort] Response from datagen failed.\n");
            close(fd);
            exit(-1);
        }

        UINT readvalues = 0;
        size_t numvalues = pow(10, atoi(Tvalue));
        size_t readbytes = 0;

        UINT *readbuf = malloc(sizeof(UINT) * numvalues);

        while (readvalues < numvalues) {
            /* read the bytestream */
            readbytes = read(fd, readbuf + readvalues, sizeof(UINT) * 1000);
            readvalues += readbytes / 4;
        }

        /* Print out the values obtained from datagen */
        printf("\nE%d:",i+1);
        for (UINT *pv = readbuf; pv < readbuf + numvalues; pv++) {
            if (pv==readbuf+numvalues-1) printf("%u.\n",*pv);
            else printf("%u,", *pv);
        }

        parallel_quicksort(readbuf,0,numvalues-1);
        
        printf("\nS%d:",i+1);
        for (UINT *pv = readbuf; pv < readbuf + numvalues; pv++) {
            if (pv==readbuf+numvalues-1) printf("%u.\n",*pv);
            else printf("%u,", *pv);
        }
        printf("\n");

        free(readbuf);
    }

    /* Issue the END command to datagen */
    int rc = strlen(DATAGEN_END_CMD);
    if (write(fd, DATAGEN_END_CMD, strlen(DATAGEN_END_CMD)) != rc) {
        if (rc > 0) fprintf(stderr, "[quicksort] partial write.\n");
        else {
            perror("[quicksort] write error.\n");
            close(fd);
            exit(-1);
        }
    }

    close(fd);
    exit(0);
}
