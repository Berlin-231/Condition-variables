/* On-demand 2-thread synchronization */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Compile with "gcc cv-sync-1.c -o cv-sync-1 -lpthread"

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
int j = 0, count = 0, i = 0, k = 0,t1runs=0,t2runs=0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int t1_count, t2_count, total_count;
void usage(char* bin_str) {
	printf("Usage: %s total_runs t1_runs t2_runs\n", bin_str);
}


void* threadrunner(void* c) {

	pthread_mutex_lock(&lock);

	if (*(int*)c == 1) { // Thread one goes inside

		for (j = 0; j < total_count;) {
			if (t1_count != 0) {
				while (i == t1_count && t1_count != 0) {
					pthread_cond_wait(&cond1, &lock);
				}
				if (count < total_count) {
					for (i; i < t1_count; i++) {
						printf("%02d :IN T1, TID=%lu\n", ++count, pthread_self());
						j++,t1runs++;
						if (j == total_count) {
							pthread_cond_signal(&cond1);
							break;
						}
					}
				}
				k = 0;
				pthread_cond_signal(&cond1);
			}
			else {
				k = 0;
				pthread_cond_signal(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}

		}
	}
	else {  // Thread two goes inside
		for (j; j < total_count;) {
			if (t2_count != 0) {
				while (k == t2_count && t2_count != 0) {
					pthread_cond_wait(&cond1, &lock);
				}
				if (count < total_count) {
					for (k; k < t2_count; k++) {
						printf("%02d :IN T2, TID=%lu\n", ++count, pthread_self());
						j++,t2runs++;
						if (j == total_count) {
							pthread_cond_signal(&cond1);
							break;
						}
					}
					i = 0;
					pthread_cond_signal(&cond1);
				}

			}
			else {
				i = 0;
				pthread_cond_signal(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
		}
	}

	pthread_mutex_unlock(&lock);
}


int main(int argc, char* argv[]) {

	if (argc != 4) {
		usage(argv[0]);
		return 0;
	}

	total_count = atoi(argv[1]);
	t1_count = atoi(argv[2]);
	t2_count = atoi(argv[3]);

	if (t1_count == 0 && t2_count == 0) {
		printf("Both arguments may not be 0 at the same time!\n");
		usage(argv[0]);
		return 0;
	}

	int a = 1, b = 2;
	k = t2_count;
	pthread_t t1, t2;

	pthread_create(&t1, NULL, threadrunner, &a);
	pthread_create(&t2, NULL, threadrunner, &b);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("In main thread: T1 printed %d times. T2 printed %d times.\n", t1runs, t2runs);

	return 0;
}
