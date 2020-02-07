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

	//testing sending pt struct for later labs
	struct pt my_pt;
	my_pt.sx = 1;
	my_pt.sy = 2;
	my_pt.vx = 3;
	my_pt.vy = 4;
	my_pt.mass = 1337;
	my_pt.next = NULL;
	my_pt.life = 100;
	my_pt.pid[30];

	printf("Type message to send to MQ, type END to exit\n");
	while(1){
		fgets(my_pt.name, 20, stdin);
		if (MQwrite(our_mq, &my_pt) == 0) {
			printf("MQwrite:  %s\n", strerror(errno));
		}
		if(strcmp(my_pt.name,"END\n") == 0){ //fgets includes newline from input
			if (MQclose(&our_mq, MQ_NAME) == 0) {
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
	struct pt *cpt;
	while(1){
		if(MQread(client_mq, cpt) == -1) {
			printf("MQread:  %s\n", strerror(errno));
		}
		else {
			printf("c_pt.name: %s\n", cpt->name);
			printf("sx = %f\n", cpt->sx);
			printf("sy = %f\n", cpt->sy);
			printf("vx = %f\n", cpt->vx);
			printf("vy = %f\n", cpt->vy);
			printf("mass = %f\n", cpt->mass);
			printf("life = %d\n", cpt->life);
			if(strcmp(cpt->name, "END\n") == 0)
			{
				if (MQclose(&client_mq, MQ_NAME) == 0) {
					printf("Client MQclose:  %s\n", strerror(errno));
				}

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

