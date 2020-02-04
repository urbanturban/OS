#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MQ_NAME "/our_mq"

static mqd_t our_mq, client_mq; //Message queue descriptors
pthread_t sthread, cthread;


void *server(void *arg){
	printf("%d\n", MQcreate(&our_mq, MQ_NAME));
	printf("MQcreate: %s\n", strerror(errno));
	char msg[20];
	while(1){
		fgets(&msg, 20, stdin);
		printf("The message is: %s\n", msg);
		MQwrite(our_mq, msg);
		printf("MQwrite:  %s\n", strerror(errno));
		if(strcmp(msg,"END\n") == 0){ //fgets includes newline from input
			printf("Exiting server thread.\n");
			pthread_exit(&sthread);
		}

	}
}
void *client(void *arg){
	sleep(1);
	printf("%d\n", MQconnect(&client_mq, MQ_NAME));
	printf("MQconnect: %s\n", strerror(errno));
	char rBuffer[1024+10];
	while(1){
		sleep(1);
		if(MQread(client_mq,(char*)rBuffer) != -1) {
			printf("Client: %s\n", rBuffer);
			printf("MQ read attempt from client:  %s\n", strerror(errno));
		}
		else printf("MQ read attempt from client:  %s\n", strerror(errno));


	}
}
/*
int main() {
	//printf("%d\n", MQcreate(&our_mq, MQ_NAME));
	//printf("MQcreate: %s\n", strerror(errno));
	pthread_create(&sthread, NULL,server, NULL);
	pthread_create(&cthread, NULL,client, NULL);
	pthread_join(sthread, NULL);
	printf("Server exited.\n");
	pthread_join(cthread, NULL);
}
*/
