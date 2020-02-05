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

//server creates MQ, takes input text from stdin, writes it to MQ, closes MQ if "END" is sent and exits thread.
void *server(void *arg){
	if (MQcreate(&our_mq, MQ_NAME) == 0) {
		printf("MQcreate: %s\n", strerror(errno));
	}
	char msg[20];
	printf("Type message to send to MQ, type END to exit\n");
	while(1){
		fgets(&msg, 20, stdin);
		printf("Server: %s", msg);
		if (MQwrite(our_mq, msg) == 0) {
			printf("MQwrite:  %s\n", strerror(errno));
		}
		if(strcmp(msg,"END\n") == 0){ //fgets includes newline from input
			if (MQclose(&our_mq, MQ_NAME) == 0) { //mq_close wont use name...redudant
				printf("MQclose:  %s\n", strerror(errno));
			}
			pthread_exit(&sthread);
		}
	}
}

//client reads from MQ, exits if END is read.
void *client(void *arg){
	if (MQconnect(&client_mq, MQ_NAME) == 0) {
		printf("MQconnect: %s\n", strerror(errno));
	}

	char rBuffer[1024+10];
	while(1){
		if(MQread(client_mq,(char*)rBuffer) == -1) {
			printf("MQread:  %s\n", strerror(errno));
		}
		else {
			printf("Client: %s\n", rBuffer);
			if(strcmp(rBuffer, "END\n") == 0){
							pthread_exit(&cthread);
						}
		}
	}
}

int main() {
	//create threads
	pthread_create(&sthread, NULL,server, NULL);
	pthread_create(&cthread, NULL,client, NULL);

	//finish
	pthread_join(sthread, NULL);
	printf("Server exited.\n");
	pthread_join(cthread, NULL);
	printf("Client exited. \n");
}

