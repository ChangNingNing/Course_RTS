#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <string.h>

#define MAXT 2

int busy_work(){
	long long int x = 0;
	for (int i=0; i<100000000; i++)
		x += i;
	return x;
}

void *myRun(void *tID){
	int ID = *(int *)tID;
	for (int i=0; i<5; i++){
		printf("Thread %d is running at phase %d/5.\n", ID, i+1);
		busy_work();
	}
	printf("Thread %d finished.\n", ID);
	pthread_exit(NULL);
}

int main(int argc, char **argv){
	pthread_t threads[MAXT];
	int tID[MAXT];

	if (argc > 1 && strncmp(argv[1], "SCHED_FIFO", 10)==0){
		cpu_set_t my_set;
		CPU_ZERO(&my_set);
		CPU_SET(0, &my_set);

		sched_setaffinity(0, sizeof(cpu_set_t), &my_set);
		printf("sched_setaffinity re = %s\n", strerror(errno));

		struct sched_param param;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);

		sched_setscheduler(0, SCHED_FIFO, &param);
		printf("sched_setscheduler re = %s\n", strerror(errno));
	}

	for (int i=0; i<MAXT; i++){
		tID[i] = i;
		pthread_create(&threads[i], NULL, myRun, (void *)(tID + i));
		printf("Thread %d was created.\n", i);
	}

	for (int i=0; i<MAXT; i++)
		pthread_join(threads[i], NULL);

	return 0;
}
