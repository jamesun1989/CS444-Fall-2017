#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_THREAD_NUM 3

struct Node{
	int value;
	struct Node *next;
}*head;

pthread_mutex_t inserter_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t deleter_lock = PTHREAD_MUTEX_INITIALIZER;

sem_t deleter_lock_searcher;
sem_t deleter_lock_inserter;

void searcher();
void inserter();
void deleter();


void searcher(){
	struct Node *cursor;
	while(1){
		sem_wait(&deleter_lock_searcher);
		cursor = head;
		if(cursor != NULL){
			printf("Searching Linked List:\n");
			while(cursor != NULL){
				printf("%d ", cursor->value);
				cursor = cursor->next;
			}
			printf("\n");
		}else{
			printf("The list is empty.\n");
		}
		sem_post(&deleter_lock_searcher);
		sleep(2);
	}
}

void inserter(){
	struct Node *cursor, **tail, *temp;
	int insert_num;
	while(1){
		sem_wait(&deleter_lock_inserter);
		pthread_mutex_lock(&inserter_lock);
		insert_num = rand()%20;
		cursor = head;
		if(cursor == NULL){
			cursor = (struct Node *)malloc(sizeof(struct Node));
			cursor->value = insert_num;
			cursor->next = NULL;
			head = cursor;
		}else{
			printf("Inserting %d to linked list.\n", insert_num);
			tail = &head;
			temp = (struct Node *)malloc(sizeof(struct Node));
			temp->value = insert_num;
			temp->next = NULL;
			while(*tail != NULL){
				tail = &((*tail)->next);
			}
			*tail = temp;
		}
		sem_post(&deleter_lock_inserter);
		pthread_mutex_unlock(&inserter_lock);
		sleep(5);
	}
}

void deleter(){
	int delete_num;
	struct Node *cursor, *prev;
	while(1){
		sem_wait(&deleter_lock_inserter);
		sem_wait(&deleter_lock_searcher);
		pthread_mutex_lock(&deleter_lock);
		cursor = head;
		while(cursor != NULL){
			delete_num = rand()%20;
			if(cursor->value == delete_num){
				printf("Deleting %d from linked list\n", delete_num);
				if(cursor == head){
					head = cursor->next;
					free(cursor);
					break;
				}else{
					prev->next = cursor->next;
					free(cursor);
					break;
				}
			}else{
					prev = cursor;
					cursor = cursor->next;
				}
			}
			pthread_mutex_unlock(&deleter_lock);
			sem_post(&deleter_lock_inserter);
			sem_post(&deleter_lock_searcher);
			sleep(5);
		}
}

int main(){
	pthread_t searcher_thread[MAX_THREAD_NUM];
	pthread_t inserter_thread[MAX_THREAD_NUM];
	pthread_t deleter_thread[MAX_THREAD_NUM];
	int i;

	sem_init(&deleter_lock_searcher, 0 ,1);
	sem_init(&deleter_lock_inserter, 0, 1);

	struct Node *root;
	root = (struct Node *)malloc(sizeof(struct Node));
	root->value = rand()%10;
	head = root;
	head->next = NULL;

	for(i=0; i<MAX_THREAD_NUM; i++){
		pthread_create(&searcher_thread[i], NULL, (void *)searcher,(void *)NULL);
		pthread_create(&inserter_thread[i], NULL, (void *)inserter, (void *)NULL);
		pthread_create(&deleter_thread[i], NULL, (void *)deleter, (void *)NULL);
	}

	for(i=0; i<MAX_THREAD_NUM; i++){
		pthread_join(searcher_thread[i], NULL);
		pthread_join(inserter_thread[i], NULL);
		pthread_join(deleter_thread[i], NULL);
	}

	return 0;
}