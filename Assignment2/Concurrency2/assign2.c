/*
*CS444 Concurrency 2
*Xiaoli Sun, ID: 932051695
*Jaydeep Hemant Rotithor, ID: 
*/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include " mt19937ar.c"

#define MAX 5

sem_t fork[MAX];
pthread_mutex_t mutex;

struct philosophor_struct{
	char philosophor_name[100];
	int philosophor_id;
};

void eat();
void think();
void get_forks();
void put_forks();
void do();

void think(struct philosophor_struct *p){
	int sleep_time = rand() % 20 + 1;

	printf("%s is hungry and waiting for %d seconds\n", p->name, sleep_time);

	sleep(sleep_time);
}

void eat(struct  philosophor_struct *p){
	int sleep_time = rand() % 8 + 2;

	printf("%s is eating noodles for %d seconds\n", p->name, sleep_time);

	sleep(sleep_time);
}

void get_forks(struct philosophor_struct *p){
	
}
