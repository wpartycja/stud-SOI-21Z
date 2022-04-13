#include <iostream>
#include <pthread.h>
#include "smart_buffer.h"
#include "smart_buffer.cpp"

SmartBuffer buffer_A("A");
SmartBuffer buffer_B("B");
SmartBuffer buffer_C("C");

void* produce_even(void* arg){
	for(int item=0; ; item+=2){
        sleep(1);
		buffer_A.produce(item);
        item+=2;
		buffer_B.produce(item);
	}
}

void* produce_odd(void* arg){
	for(int item=1; ; item+=2){
        sleep(1);
		buffer_C.produce(item);

        item+=2;

		buffer_B.produce(item);
	}
}

void* consume_A(void* arg){
	while(1){
        sleep(1);
		buffer_A.consume();
    }
}

void* consume_B(void* arg){
	while(1){
        sleep(1);
		buffer_B.consume();
    }
}

void* consume_C(void* arg){
	while(1){
        sleep(1);
		buffer_C.consume();
    }
}


int main(){
	pthread_t producer_even, producer_odd;
    pthread_t consumer_A, consumer_B, consumer_C;

	pthread_create(&producer_even, NULL, produce_even, NULL);
    pthread_create(&producer_odd, NULL, produce_odd, NULL);
	pthread_create(&consumer_A, NULL, consume_A, NULL);
    pthread_create(&consumer_B, NULL, consume_B, NULL);
    pthread_create(&consumer_C, NULL, consume_C, NULL);

	pthread_exit(NULL);
	return 0;
}