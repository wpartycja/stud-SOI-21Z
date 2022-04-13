#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <random>
#include <unistd.h>
#include <queue>

using namespace std;

#define BUFFER_SIZE 5

queue<int> buffer_A;
queue<int> buffer_B;
queue<int> buffer_C;

sem_t full_A, empty_A;
pthread_mutex_t mutex_A;

sem_t full_B, empty_B;
pthread_mutex_t mutex_B;

sem_t full_C, empty_C;
pthread_mutex_t mutex_C;

void semaphores_init(){
	sem_init(&empty_A, 0, 0);
	sem_init(&full_A, 0, BUFFER_SIZE);

    sem_init(&empty_B, 0, 0);
	sem_init(&full_B, 0, BUFFER_SIZE);

    sem_init(&empty_C, 0, 0);
	sem_init(&full_C, 0, BUFFER_SIZE);

	pthread_mutex_init(&mutex_A, NULL);
    pthread_mutex_init(&mutex_B, NULL);
    pthread_mutex_init(&mutex_C, NULL);

}

void* produce_even(void* arg){
	for(int item=0; ; item+=2){
        sleep(1);
		sem_wait(&full_A);
		pthread_mutex_lock(&mutex_A);
		buffer_A.push(item);
		cout<<"Produced to buffer A: "<< item << "; Buffer A size: " << buffer_A.size() << endl;
		pthread_mutex_unlock(&mutex_A);
		sem_post(&empty_A);

        item+=2;

        sem_wait(&full_B);
		pthread_mutex_lock(&mutex_B);
		buffer_B.push(item);
		cout<<"Produced to buffer B: "<< item << "; Buffer B size: " << buffer_B.size() << endl;
		pthread_mutex_unlock(&mutex_B);
		sem_post(&empty_B);
	}
}

void* produce_odd(void* arg){
	for(int item=1; ; item+=2){
        sleep(1);
        sem_wait(&full_C);
		pthread_mutex_lock(&mutex_C);
		buffer_C.push(item);
		cout<<"Produced to buffer C: "<< item << "; Buffer C size: " << buffer_C.size() << endl;
		pthread_mutex_unlock(&mutex_C);
		sem_post(&empty_C);

        item+=2;

		sem_wait(&full_B);
		pthread_mutex_lock(&mutex_B);
		buffer_B.push(item);
		cout<<"Produced to buffer B: "<<item<< "; Buffer B size: " << buffer_B.size() << endl;
		pthread_mutex_unlock(&mutex_B);
		sem_post(&empty_B);

        item+=2;
	}
}


void* consume_A(void* arg){
	while(1){
        sleep(1);
		sem_wait(&empty_A);
		pthread_mutex_lock(&mutex_A);
		int item = buffer_A.front();
        buffer_A.pop();
		cout<<"Got by consument A: "<<item<< "; Buffer A size: " << buffer_A.size() << endl;
		pthread_mutex_unlock(&mutex_A);
		sem_post(&full_A);
    }
}

void* consume_B(void* arg){
	while(1){
        sleep(1);
		sem_wait(&empty_B);
		pthread_mutex_lock(&mutex_B);
		int item = buffer_B.front();
        buffer_B.pop();
		cout<<"Got by consument B: "<<item<<"; Buffer B size: " << buffer_B.size() << endl;
		pthread_mutex_unlock(&mutex_B);
		sem_post(&full_B);
	}
}

void* consume_C(void* arg){
	while(1){
        sleep(1);
		sem_wait(&empty_C);
		pthread_mutex_lock(&mutex_C);
		int item = buffer_C.front();
        buffer_C.pop();
		cout<<"Got by consument C: "<<item<< "; Buffer C size: " << buffer_C.size() << endl;
		pthread_mutex_unlock(&mutex_C);
		sem_post(&full_C);
	}
}



int main(){
	pthread_t producer_even, producer_odd;
    pthread_t consumer_A, consumer_B, consumer_C;

	semaphores_init();

	pthread_create(&producer_even, NULL, produce_even, NULL);
    pthread_create(&producer_odd, NULL, produce_odd, NULL);
	pthread_create(&consumer_A, NULL, consume_A, NULL);
    pthread_create(&consumer_B, NULL, consume_B, NULL);
    pthread_create(&consumer_C, NULL, consume_C, NULL);

	pthread_exit(NULL);
	return 0;
}