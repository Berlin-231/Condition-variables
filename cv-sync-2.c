/* On-demand 3-thread synchronization */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Compile with "gcc cv-sync-2.c -o cv-sync-2 -lpthread"
int global_index = 1, t1runs = 0, t2runs = 0, t3runs = 0;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
int j = 0, count = 0, i = 0, k = 0, l = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int t1_count, t2_count, t3_count, total_count;
int isfree = 1;
void usage(char* bin_str) {
	printf("Usage: %s total_runs t1_runs t2_runs t3_runs\n", bin_str);
}


void* threadrunner(void* c) {

	pthread_mutex_lock(&lock);
	if (*(int*)c == 1) {// Thread 1 goes inside
		global_index = 2;  
		for (j = 0; j < total_count;) {
			if (t1_count != 0) {
				while (i == t1_count) {
					pthread_cond_broadcast(&cond1);
					pthread_cond_wait(&cond1, &lock);

				}
				if (j < total_count) {
					for (i; i < t1_count; i++) {
						printf("%02d :IN T1, TID=%lu\n", ++count, pthread_self());
						j++, t1runs++;
						if (j == total_count) {
							l = 0, k = 0;
							pthread_cond_broadcast(&cond1);
							break;
						}
					}
					k = 0;
					pthread_cond_broadcast(&cond1);
				}
				else {
					l = 0, k = 0;
					pthread_cond_broadcast(&cond1);
				}

			}
			else if (l != 0 || t3_count == 0) {
				k = 0;
				pthread_cond_broadcast(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
			else {
				pthread_cond_broadcast(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
		}
		pthread_cond_broadcast(&cond1);

	}
	else if (*(int*)c == 2) { // Thread 2 goes in.
		global_index = 3;

		for (j; j < total_count;) {
			if (t2_count != 0) {
				while (k == t2_count || global_index == 1) {
					pthread_cond_broadcast(&cond1);
					pthread_cond_wait(&cond1, &lock);
				}
				if (j < total_count) {
					for (k; k < t2_count; k++) {
						printf("%02d :IN T2, TID=%lu\n", ++count, pthread_self());
						j++, t2runs++;
						if (j == total_count) {
							i = 0, l = 0;
							pthread_cond_broadcast(&cond1);
							break;
						}
					}

					l = 0;
					pthread_cond_broadcast(&cond1);

				}
				else {
					i = 0, l = 0;
					pthread_cond_broadcast(&cond1);
				}
			}
			else if (i != 0 || t1_count == 0) {
				l = 0;
				pthread_cond_broadcast(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
			else {
				pthread_cond_broadcast(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
		}
		pthread_cond_broadcast(&cond1);


	}
	else {
		for (j; j < total_count;) {
			if (t3_count != 0) {
				while (l == t3_count || global_index == 1 || global_index == 2) {
					pthread_cond_broadcast(&cond1);
					pthread_cond_wait(&cond1, &lock);
				}
				if (j < total_count) {
					for (l; l < t3_count; l++) {
						printf("%02d :IN T3, TID=%lu\n", ++count, pthread_self());
						j++, t3runs++;
						if (j == total_count) {
							i = 0, k = 0;
							pthread_cond_broadcast(&cond1);
							break;
						}
					}

					i = 0;
					pthread_cond_broadcast(&cond1);

				}
				else {
					i = 0, k = 0;
					pthread_cond_broadcast(&cond1);
				}
			}
			else if (global_index == 2) {
				//i = 0;
				pthread_cond_broadcast(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
			else if (k != 0 || t2_count == 0) {
				i = 0;
				pthread_cond_broadcast(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
			else {
				pthread_cond_broadcast(&cond1);
				pthread_cond_wait(&cond1, &lock);
			}
		}

		pthread_cond_broadcast(&cond1);


	}
	pthread_mutex_unlock(&lock);
}


int main(int argc, char* argv[]) {

	if (argc != 5) {
		usage(argv[0]);
		return 0;
	}


	total_count = atoi(argv[1]);
	t1_count = atoi(argv[2]);
	t2_count = atoi(argv[3]);
	t3_count = atoi(argv[4]);

	if (t1_count == 0 && t2_count == 0 && t3_count == 0) {
		printf("All three parameters may not be 0 at the same time\n");
		usage(argv[0]);
		return 0;
	}

	k = t2_count;
	l = t3_count;


	int a = 1, b = 2, c = 3;
	pthread_t t1, t2, t3;


	pthread_create(&t1, NULL, threadrunner, &a);
	pthread_create(&t2, NULL, threadrunner, &b);
	pthread_create(&t3, NULL, threadrunner, &c);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);


	printf("In main thread: T1 printed %d times. T2 printed %d times. T3 printed %d times.\n", t1runs, t2runs, t3runs);

	return 0;
}
