/*
 ============================================================================
 Name        : Main.c
 Author      : Akash Menon
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : Basic 2 thread, process parsing message using POSIX MQ
 ============================================================================
 */
#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MQ_NAME "/our_mq" // Name of MQ that both client and server share.

static mqd_t server_mqd, client_mqd; //Message queue descriptors
pthread_t sthread, cthread;

//server creates MQ, takes input text from stdin, writes it to MQ, closes MQ if "END" is sent and exits thread.
void *server(void *arg){
	if (MQcreate(&server_mqd, MQ_NAME) == 0) {
		printf("MQcreate: %s\n", strerror(errno));
	}
	// Sending pt structs to accomodate the future lab
	struct pt my_pt;
	my_pt.sx = 1;
	my_pt.sy = 2;
	my_pt.vx = 3;
	my_pt.vy = 4;
	my_pt.mass = 1337;
	my_pt.next = NULL;
	my_pt.life = 100;

	printf("Type message to send to MQ, type END to exit\n");
	while(1){
		fgets(my_pt.name, 20, stdin);
		if (MQwrite(server_mqd, &my_pt) == 0) {
			printf("MQwrite:  %s\n", strerror(errno));
		}
		if(strcmp(my_pt.name,"END\n") == 0){ //fgets includes newline from input
			if (MQclose(&server_mqd, MQ_NAME) == 0) {
				printf("MQclose:  %s\n", strerror(errno));
			}
			pthread_exit(&sthread);
		}
	}
}

//client reads from MQ, prints the contents of the struct and exits if END is read.
void *client(void *arg){
	if (MQconnect(&client_mqd, MQ_NAME) == 0) {
		printf("MQconnect: %s\n", strerror(errno));
	}
	struct pt *cpt;
	cpt = malloc(sizeof(struct pt));
	while(1){
		if(MQread(client_mqd, (void*)cpt) == -1) {
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
				if (MQclose(&client_mqd, MQ_NAME) == 0) {
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

	//wait for threads to finish.
	pthread_join(sthread, NULL);
	printf("Server exited.\n");
	pthread_join(cthread, NULL);
	printf("Client exited. \n");
}

