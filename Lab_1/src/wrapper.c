/*
 ============================================================================
 Name        : wrapper.c
 Author      : Akash Menon
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : Wrapper encapsulating POSIX Message Queue calls.
 ============================================================================
 */
#include "wrapper.h"

#define MAX_SIZE 1024 //msg size
#define MAX_MESSAGES 100

//Big note! If you think the arguments makes no sense, you are allowed to change them, as long as the basic functionality is kept
//In case you run in to blocking issues with reading from the queue, the option O_NONBLOCK is a hot tip

int MQcreate (mqd_t * mq, char * name)
{ 	//Should create a new messagequeue, use mq as reference pointer so you can reach the handle from anywhere
	//Should return 1 on success and 0 on fail
	static struct mq_attr attr;
	//attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MESSAGES;
	attr.mq_msgsize = MAX_SIZE;
	//attr.mq_curmsgs = 0;
	*mq = mq_open(name, O_CREAT | O_RDWR | O_NONBLOCK, 0777, &attr);
	if(*mq != -1) return 1;
	else return 0;

}
int MQconnect (mqd_t * mq, char * name)
{
    /* Connects to an existing mailslot for writing Uses mq as reference pointer, so that you can 	reach the handle from anywhere*/
    /* Should return 1 on success and 0 on fail*/
	*mq = mq_open(name, O_RDONLY);
	if(*mq != -1) return 1;
	else return 0;
}

//void * MQread (mqd_t mq, void ** buffer)
void * MQread (mqd_t mq, void ** buffer)
{
    /* Read a msg from a mailslot, return nr Uses mq as reference pointer, so that you can 		reach the handle from anywhere */
    /* should return number of bytes read              */
	int nbytes;
	nbytes = mq_receive(mq, (void*)buffer, MAX_SIZE, (void*)NULL);
	return nbytes;

}

int MQwrite (mqd_t mq, void * sendBuffer)
{
    /* Write a msg to a mailslot, return nr Uses mq as reference pointer, so that you can 	     reach the handle from anywhere*/
    /* should return number of bytes read         */
	int status;
	struct pt *p_pt = sendBuffer;
	printf("size of received struct: %ld\n", sizeof(*p_pt));
	status = mq_send(mq, (char*)p_pt, sizeof(*p_pt), 0);
	//TODO return no of bytes read?
	if(status != -1) return 1;
	else return 0;
}

int MQclose(mqd_t * mq, char * name)
{
    /* close a mailslot, returning whatever the service call returns Uses mq as reference pointer, so that you can
    reach the handle from anywhere*/
    /* Should return 1 on success and 0 on fail*/
	int status;
	status = mq_close(*mq);
	//mq_close(*mq);
	//status = mq_unlink(name);
	if(status != -1) return 1;
	else return 0;
}



