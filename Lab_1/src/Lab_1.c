/*
 ============================================================================
 Name        : Lab_1.c
 Author      : Jakob Danielsson
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "wrapper.h"
#include <pthread.h>
#include <unistd.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *hello_moon(void *arg){
	for(int i = 0; i<10; i++) {
		pthread_mutex_lock(&mutex);
		printf("Hello Moon \n");
		pthread_mutex_unlock(&mutex);
		sleep(0.2);
	}

}

void *hello_world(void *arg){
	for(int i = 0; i < 10; i++) {
		pthread_mutex_lock(&mutex);
		printf("Hello world \n");
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}
/*
int main(int ac, char * argv)
{

	while(1){
		pthread_t thread1, thread2;
		pthread_create(&thread1, NULL, hello_world, NULL);
		pthread_create(&thread2, NULL, hello_moon, NULL);
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
	}

}
*/

