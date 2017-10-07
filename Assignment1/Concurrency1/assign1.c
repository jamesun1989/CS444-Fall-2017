#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mt19937ar.c"
#include <unistd.h>
#include <time.h>

#define max_buffer_size 32

struct numbers{//numbers steuct that contains two integers, one is for num and the second is for waiting period
	int num;
	int random_waiting_period;
};

int buffer[max_buffer_size];//initialize a buffer array with the size of 32
int buffer_size = 0;//define a global variable of buffer size.

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//initialize three pthread variable
pthread_cond_t consumer_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t producer_condition = PTHREAD_COND_INITIALIZER;

void* produce(void* args){//produce numbers
	while(1){
		int rand_waiting_time, producer_number, producer_rand_waiting_time;
		struct numbers producer_number_period;
		producer_rand_waiting_time = genrand_int32() % 5 + 3;
		sleep(producer_rand_waiting_time);//wait 3 to 7 seconds to produce numbers

		pthread_mutex_lock(&mutex);

		while(buffer_size == max_buffer_size){//check if buffer size is max
			pthread_cond_wait(&producer_condition, &mutex);//if the buffer size is max, don't add number to it and wait until there are rooms in buffer array.
		}

		producer_number = genrand_int32() % 100 + 1;//random numbers that produced by producer
		rand_waiting_time = genrand_int32() % 8 + 2;//random waiting time between 2 to 9 seconds

		producer_number_period.num = producer_number;//assign the two numbers to struct
		producer_number_period.random_waiting_period = rand_waiting_time;

		buffer_size++;//after adding a number to the buffer, buffer size need to add one
		buffer[buffer_size] = producer_number;//put random generated number to the buffer

		printf("Produced a number %d and wait %d\n", producer_number, rand_waiting_time);
		printf("The buffer size is %d\n\n", buffer_size);

		pthread_cond_signal(&consumer_condition);
		pthread_mutex_unlock(&mutex);
	}
}

void* consume(void* args){//consumer
	while(1){
		int consumer_rand_waiting_time;
		consumer_rand_waiting_time = genrand_int32() % 8 + 2;
		//sleep(consumer_rand_waiting_time);

		pthread_mutex_lock(&mutex);

		while(buffer_size == 0){//check to see if the buffer size equal to 0 or not
			pthread_cond_wait(&consumer_condition, &mutex);
		}

		pthread_mutex_unlock(&mutex);

		sleep(consumer_rand_waiting_time);

		pthread_mutex_lock(&mutex);

		buffer_size--;//delete a number from buffer will cause buffer size minus one

		printf("Deleted a number %d and wait %d\n", buffer[buffer_size+1], consumer_rand_waiting_time);
		printf("The buffer size is: %d\n\n", buffer_size);

		pthread_cond_signal(&producer_condition);
		pthread_mutex_unlock(&mutex);
	}
}

int main(){
	pthread_t producer, consumer;

	pthread_mutex_init(&mutex, NULL);//initialize pthread
	pthread_cond_init(&producer_condition, NULL);
	pthread_cond_init(&consumer_condition, NULL);

	pthread_create(&producer, NULL, produce, NULL);//create pthread
	pthread_create(&consumer, NULL, consume, NULL);

	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);
}
