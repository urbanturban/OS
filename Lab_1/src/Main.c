#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define MQ_NAME "/our_mq"

static mqd_t our_mq;
pthread_t sthread, cthread;

int main() {
	printf("%d\n", MQcreate(&our_mq, MQ_NAME));
	printf("MQ status %s\n", strerror(errno));
}

void *server(void *arg){

}
void *client(void *arg){

}

