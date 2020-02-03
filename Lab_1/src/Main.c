#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define MQ_NAME "/our_mq"

static mqd_t our_mq;
pthread_t sthread, cthread;

void *server(void *arg){
	printf("%d\n", MQcreate(&our_mq, MQ_NAME));
	printf("MQ status from server %s\n", strerror(errno));
	while(1){
		//TODO
	}
}
void *client(void *arg){
	printf("%d\n", MQconnect(&our_mq, MQ_NAME));
	printf("MQ status from client  %s\n", strerror(errno));
	while(1){
		//TODO
	}
}

int main() {
	pthread_create(&sthread, NULL,server, NULL);
	pthread_create(&cthread, NULL,client, NULL);

	pthread_join(sthread, NULL);
	pthread_join(cthread, NULL);
}

