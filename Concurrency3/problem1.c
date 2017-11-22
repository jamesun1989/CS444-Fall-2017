/*
*	CS444 Concurrency 3 Problem1
*	Xiaoli Sun 				ID:932051695
*	Jaydeep Hemant Rotithor ID:931862831
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define THREAD_NUM 10

//set default number of threads to 0
int thread_counter = 0;
//0 means the shareable resource is not full, othrwise 1
int sharable_resource_full = 0;
//semaphores for process
sem_t sem;

void *process(void *thread_id);

void *process(void *thread_id){
	int id = *((int *)thread_id);//Thread id could be used later
	int i;

	while(1){
		sem_wait(&sem);
		thread_counter++;//increment thread number for shareable resource
		if(thread_counter == 3){//if total number of threads in shareable resource, set the resource to 1, otherwise set to 0
			sharable_resource_full = 1;
		}else{
			sharable_resource_full = 0;
		}
		
		printf("Thread %d is using resource\n", id);//print the id of process which is using the resource
		sleep(rand() % 5);

		printf("\nThread %d is killed\n", id);
		printf("\n");
		if(sharable_resource_full == 1){//if total number of threads that using shareable resource reaches 3, decrement it, if equal to 0, unlock the resource and add new threads to it again
			thread_counter--;
			if(thread_counter == 0){
				sem_post(&sem);
				sem_post(&sem);
				sem_post(&sem);
			}
		}else{
			thread_counter--;
			sem_post(&sem);
		}
	}
}

int main(){
	pthread_t threads[THREAD_NUM];
	sem_init(&sem, 0, 3);
	int i;

	for (i = 0; i < THREAD_NUM; i++){
		pthread_create(&threads[i], NULL, process, &i);
	}

	for(i = 0; i < THREAD_NUM; i++){
		pthread_join(threads[i], NULL);
	}

	return 0;
}