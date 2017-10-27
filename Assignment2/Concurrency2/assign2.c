/*
*CS444 Concurrency 2
*Xiaoli Sun, ID: 932051695
*Jaydeep Hemant Rotithor, ID: 931862831
*/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define MAX 5

sem_t forks[MAX];
pthread_mutex_t mutex;

struct philosophor_struct{//a construct that contains an array for philosopher and id
	char philosophor_name[100];
	int philosophor_id;
};

void eat();
void think();
void get_forks();
void put_forks();
void *doit();

void think(struct philosophor_struct *p){//it will take 1 to 20 secs for thinking
	int sleep_time = rand() % 20 + 1;

	pthread_mutex_lock(&mutex);
	printf("%s is hungry.\n", p->philosophor_name);
	pthread_mutex_unlock(&mutex);

	sleep(sleep_time);
}

void eat(struct  philosophor_struct *p){//will take 2 to 9 seconds for eating
	int sleep_time = rand() % 8 + 2;

	pthread_mutex_lock(&mutex);
	printf("%s is eating noodles.\n", p->philosophor_name);
	pthread_mutex_unlock(&mutex);

	sleep(sleep_time);
}

void get_forks(struct philosophor_struct *p){
	int left_fork = (p->philosophor_id + 5) % MAX;//if p_id is 0, the left fork will be 0. if p_id is 4, the left fork will be 4
	int right_fork = (p->philosophor_id + 1) % MAX;//if p_id is 0, the right fork will be 1. If p_id is 4, the it's right fork is 0
	int sem_value;
	int forks_check = 1;

	while(forks_check){//check the left fork to see if it is being used or not
		sem_getvalue(&forks[left_fork], &sem_value);

		if(sem_value == 1){//if left fork is not being used, take it.
			sem_wait(&forks[left_fork]);//lock the left fork 

			sem_getvalue(&forks[right_fork], &sem_value);//then check to see if right fork is being used or not
			if(sem_value == 1){//if it is not being used, take it as well.
				sem_wait(&forks[right_fork]);//lock the second fork

				forks_check = 0;
			}else{
				sem_post(&forks[left_fork]);//if not, unlock the left fork
			}
		}
	}

	printf("%s get forks %d and %d.\n", p->philosophor_name, left_fork, right_fork);

	
}

void put_forks(struct philosophor_struct *p){
	int left_fork = (p->philosophor_id + 5) % MAX;
	int right_fork = (p->philosophor_id + 1) % MAX;

	pthread_mutex_lock(&mutex);
	printf("%s puts down forks %d and %d\n", p->philosophor_name, left_fork, right_fork);
	pthread_mutex_unlock(&mutex);

	sem_post(&forks[right_fork]);
	sem_post(&forks[left_fork]);
}

void *doit(void *p){
	struct philosophor_struct *philo = (struct philosophor_struct *)p;

	while(1){
		think(philo);
		get_forks(philo);
		eat(philo);
		put_forks(philo);
	}
}

int main(){
	int i;

	pthread_t p_thread[MAX];

	struct philosophor_struct *philo;

	philo = malloc(sizeof(struct philosophor_struct)*5);

	strcpy(philo[0].philosophor_name, "Kevin");//copy names to each philosophor
	strcpy(philo[1].philosophor_name, "Xiaoli");
	strcpy(philo[2].philosophor_name, "Jaydeep");
	strcpy(philo[3].philosophor_name, "Batman");
	strcpy(philo[4].philosophor_name, "Ironman");

	pthread_mutex_init(&mutex, NULL);

	for(i=0;i<MAX;i++){//initiate semaphore
		sem_init(&forks[i], 0, 1);
	}

	for(i=0;i<MAX;i++){//create a new thread
		philo[i].philosophor_id = i;
		pthread_create(&p_thread[i], NULL, doit, &philo[i]);
	}

	for(i=0;i<MAX;i++){
		pthread_join(p_thread[i], NULL);
	}
	return 0;
}