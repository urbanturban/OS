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
	char msg[20];
	printf("%d\n", MQcreate(&our_mq, MQ_NAME));
	printf("MQ creation status from server %s\n", strerror(errno));
	while(1){
		fgets(&msg, 20, stdin);
		printf("The message is: %s\n", msg);
		printf("Write Attempt Code: %d\n", MQwrite(our_mq, msg));
		printf("MQ write attempt from server:  %s\n", strerror(errno));
		if(strcmp(msg,"END\n") == 0){ //fgets includes newline from input
			printf("Exiting server thread.\n");
			pthread_exit(&sthread);
		}

	}
}
void *client(void *arg){
	sleep(1);
	printf("%d\n", MQconnect(&client_mq, MQ_NAME));
	printf("MQ connection status from client  %s\n", strerror(errno));
	char rBuffer[2048];
	while(1){
		sleep(1);
		printf("Number of bytes read: %d\n", MQread(client_mq,&rBuffer));
		printf("MQ read attempt from client:  %s\n", strerror(errno));
		printf("Client: Msg received is %s\n", rBuffer);
	}
}

int main() {
	pthread_create(&sthread, NULL,server, NULL);
	pthread_create(&cthread, NULL,client, NULL);

	pthread_join(sthread, NULL);
	printf("Server exited.\n");
	pthread_join(cthread, NULL);
}

