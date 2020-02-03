#include "wrapper.h"

#define MAX_SIZE 1024 //msg size
#define MAX_MESSAGES 10

//Big note! If you think the arguments makes no sense, you are allowed to change them, as long as the basic functionality is kept
//In case you run in to blocking issues with reading from the queue, the option O_NONBLOCK is a hot tip

int MQcreate (mqd_t * mq, char * name)
{ 	//Should create a new messagequeue, use mq as reference pointer so you can reach the handle from anywhere
	//Should return 1 on success and 0 on fail
	struct mq_attr our_mq_attr;
	our_mq_attr.mq_msgsize = MAX_SIZE;
	our_mq_attr.mq_maxmsg = MAX_MESSAGES;

	mq = mq_open(name, O_CREAT|O_RDWR|O_NONBLOCK,0666, &our_mq_attr);
	if(mq != -1) return 1;
	else return 0;

}
int MQconnect (mqd_t * mq, char * name)
{
    /* Connects to an existing mailslot for writing Uses mq as reference pointer, so that you can 	reach the handle from anywhere*/
    /* Should return 1 on success and 0 on fail*/

	mq = mq_open(name, O_RDONLY);
	if(mq != -1) return 1;
	else return 0;
}

void * MQread (mqd_t mq, void ** buffer)
{

    /* Read a msg from a mailslot, return nr Uses mq as reference pointer, so that you can 		reach the handle from anywhere */
    /* should return number of bytes read              */
}

int MQwrite (mqd_t mq, void * sendBuffer)
{
    /* Write a msg to a mailslot, return nr Uses mq as reference pointer, so that you can 	     reach the handle from anywhere*/
    /* should return number of bytes read         */
	return 1;
}

int MQclose(mqd_t * mq, char * name)
{
    /* close a mailslot, returning whatever the service call returns Uses mq as reference pointer, so that you can
    reach the handle from anywhere*/
    /* Should return 1 on success and 0 on fail*/
	return 1;
}



