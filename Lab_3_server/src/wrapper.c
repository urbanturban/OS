/*
 * wrapper.c
 *
 *  Created on: Jan 20, 2020
 *      Author: student
 */


#include "wrapper.h"

#define MAX_SIZE 1024

//Big note! If you think the arguments makes no sense, you are allowed to change them, as long as the basic functionality is kept
//In case you run in to blocking issues with reading from the queue, the option O_NONBLOCK is a hot tip

int MQcreate(mqd_t * mq, char * name) {
	static struct mq_attr attr;
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = MAX_SIZE;


	*mq = mq_open(name, O_CREAT | O_RDWR, 0777, &attr);	//Should create a new messagequeue, use mq as reference pointer so you can reach the handle from anywhere
	if (*mq != -1)
		return 1;	//Should return 1 on success and 0 on fail
	else {
		printf("error create queue: %s", strerror(errno));
		return 0;
	}
}
int MQconnect(mqd_t * mq, char * name) {
	*mq = mq_open(name, O_RDWR); /* Connects to an existing mailslot for writing Uses mq as reference pointer, so that you can 	reach the handle from anywhere*/
	if (*mq != -1)
		return 1;	//Should return 1 on success and 0 on fail
	else
		return 0;/* Should return 1 on success and 0 on fail*/
}

void * MQread(mqd_t mq, void ** buffer) {
	int bytes;
	struct mq_attr attr;
	mq_getattr(mq, &attr);

	bytes = mq_receive(mq, (char*) *buffer, attr.mq_msgsize, NULL);	//
	if (bytes == -1) {
		//printf("error read. %s\n", strerror(errno));
		return 0;
	}
	/* Read a msg from a mailslot, return nr Uses mq as reference pointer, so that you can 		reach the handle from anywhere */
	return bytes;/* should return number of bytes read              */
}

int MQwrite(mqd_t mq, void * sendBuffer) {

	if (mq_send(mq, sendBuffer, sizeof(*(planet_type*)sendBuffer), NULL) == 0) {
		return 1;
	}
	else {
		printf(" error write. %s\n", strerror(errno));
		return 0;
	}
	/* Write a msg to a mailslot, return nr Uses mq as reference pointer, so that you can 	     reach the handle from anywhere*/
	/* should return number of bytes read         */
}

int MQclose(mqd_t * mq, char * name) {
	mq_close(*mq);/* close a mailslot, returning whatever the service call returns Uses mq as reference pointer, so that you can
	 reach the handle from anywhere*/
	if (*mq == -1) {
		printf(" error close. %s\n", strerror(errno));
		return 0;
	}/* Should return 1 on success and 0 on fail*/
	else
		return 1;
}
