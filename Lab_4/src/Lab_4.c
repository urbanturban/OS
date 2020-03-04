/*
 ============================================================================
 Name        : OSsched.c
 Author      : Jakob Danielsson
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : RR scheduler simulator, Ansi-style
 ============================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<time.h>

#define sched_RR 1
#define sched_SJF 2
#define sched_MQ 3
#define sched_EDF 4

#define CUSTOM_TASKSET 2
#define DEFAULT_TASKSET 0

#define QUEUE_SIZE 10
int task_set = DEFAULT_TASKSET;
int sched_type = sched_EDF;
int finished = 0;
int context_switch_program_exit = 0;
int context_switch = 0;
int OS_cycles = 0;
int context_switches = 0;


typedef struct taskprop{
    int deadline;		//Deadline of a task, not necesarry to use
    int period;			//Periodicity of a task, not necesarry to use if you dont want to create a periodic scheduler such as rate monotonic. If you want to do this, talk with Jakob first
    int release_time;		//The time when a task is supposed to start, i.e., released from waiting queue
    int priority;		//Priority of task, can be used for the multiple queues
    int ID;			//ID, to distinguish different tasks from eachother
    int quantum;		//How long the task has left to execute
    int queue_size;
    struct taskprop * next;
    struct taskprop * previous;
} task;

task * ready_queue = NULL;
task * waiting_queue = NULL;
task * exec_task;
task * idle_task;

task * high_queue; // Multilevel Queues
task * medium_queue;
task * low_queue;



task tasks[QUEUE_SIZE]; //Queue
int activeTasks = 0;
int idleTasks = 0;

//Implementera insertfunktioner

void sorted_insert(task** head, task* new_task){ //inserts into list queue based on quantum of the task. Ascending order.
	if(*head == NULL || (*head)->quantum >=new_task->quantum){ //when new_task is first task in list or if new_task is shortest job.
		new_task->next = *head;
		*head = new_task;
	}
	else {
		// Find the task that is before the point of insertion of new_task.
		task* cursor = *head;
		while(cursor->next != NULL && cursor->next->quantum < new_task->quantum){
			cursor = cursor->next;
		}
		new_task->next = cursor->next;
		cursor->next = new_task;
	}
}
void sorted_insert2(task** head, task* new_task){ //ugly quick fix for implementing EDF
	if(*head == NULL || (*head)->deadline >=new_task->deadline){ //when new_task is first task in list or if new_task is shortest job.
		new_task->next = *head;
		*head = new_task;
	}
	else {
		// Find the task that is before the point of insertion of new_task.
		task* cursor = *head;
		while(cursor->next != NULL && cursor->next->deadline < new_task->deadline){
			cursor = cursor->next;
		}
		new_task->next = cursor->next;
		cursor->next = new_task;
	}
}
void sort(task **head, int sched_type){ // take in head of linked list and sort it.
	task* sorted = NULL; //init sorted list

	task* cursor = *head; //traverse the list and insert every task into Sorted list.
	while(cursor != NULL){
		task* next = cursor->next; //save ref to next task
		if(sched_type == sched_SJF){
			sorted_insert(&sorted, cursor);
		}
		else if(sched_type == sched_EDF){
			sorted_insert2(&sorted, cursor);
		}
		cursor = next;
	}
	*head = sorted;	//make head point to sorted list
}


//------------------Linked list functions------------------
void copy_task(task ** dest, task * src)		//Copies data of a task to another task
{
	task * temp = *dest;
	temp->ID = src->ID;
	temp->deadline = src->deadline;
	temp->release_time = src->release_time;
	temp->period = src->period;
	temp->priority = src->priority;
	temp->quantum = src->quantum;
	*dest = temp;
}



task* create(int ID, int deadline, int release_time, int period, int prio, int quantum, task* next)			//Creates a new task
{
    task* new_node = (task*)malloc(sizeof(task));
    if(new_node == NULL)
    {
        printf("Error creating a new node.\n");
        exit(0);
    }
    new_node->ID = ID;
    new_node->deadline = deadline;
    new_node->release_time = release_time;
    new_node->period = period;
    new_node->priority = prio;
    new_node->quantum = quantum;
    new_node->next = next;
    return new_node;
}
task * push(task * head, task data)			//Appends a task to a list
{
    /* go to the last node */
    task *cursor = head;
    if (cursor == NULL)
    {
    	head =  create(data.ID, data.deadline, data.release_time, data.period, data.priority, data.quantum ,NULL);
    }
    else
    {
    	while(cursor->next != NULL)
    	{
    		cursor = cursor->next;
    	}
    /* create a new node */
    	task* new_node =  create(data.ID, data.deadline, data.release_time, data.period, data.priority, data.quantum ,NULL);
    	cursor->next = new_node;
    }
    return head;
}


task * pop(task * head)			//Pops the first element of a list
{
    if(head == NULL)
        return NULL;
    task *front = head;
    head = head->next;
    front->next = NULL;
    /* is this the last node in the list */
    if(front == head)
        head = NULL;
    free(front);
    return head;
}

task * remove_back(task* head)			//Removes the back of a list
{
    if(head == NULL)
        return NULL;

    task *cursor = head;
    task *back = NULL;
    while(cursor->next != NULL)
    {
        back = cursor;
        cursor = cursor->next;
    }
    if(back != NULL)
        back->next = NULL;

    /* if this is the last node in the list*/
    if(cursor == head)
        head = NULL;

    free(cursor);

    return head;
}

task* remove_front(task* head)
{
    if(head == NULL)
        return NULL;
    task *front = head;
    head = head->next;
    front->next = NULL;
    /* is this the last node in the list */
    if(front == head)
        head = NULL;
    free(front);
    return head;
}
task * remove_node (task* head,task* nd)
{
    /* if the node is the first node */
    if(nd == head)
    {
        head = pop(head);
        return head;
    }

    /* if the node is the last node */
    if(nd->next == NULL)
    {
        head = remove_back(head);
        return head;
    }

    /* if the node is in the middle */
    task* cursor = head;
    while(cursor != NULL)
    {
        if(cursor->next == nd)
            break;
        cursor = cursor->next;
    }

    if(cursor != NULL)
    {
    	task* tmp = cursor->next;
        cursor->next = tmp->next;
        tmp->next = NULL;
        free(tmp);
    }
    return head;
}
task * first_to_last (task * head)
{
	if (head==NULL)
		return NULL;
	if (head->next == NULL)
		return head;

	task * new_front = head->next;
	task* cursor = new_front;
	while(cursor->next != NULL)
	{
		cursor = cursor->next;
	}
	cursor->next = head;
	head->next=NULL;
	return new_front;

}
//------------------Reads a taskset from file and inserts them to ready queue------------------
void readTaskset_n(char * filepath)
{
	FILE *reads;											//File handle
	char * sp;
	if(task_set == CUSTOM_TASKSET){
		sp = "./taskset.txt";

	}
	else{
		sp = "/home/student/Desktop/Labs_material/taskset.txt";								//File path
	}
    reads=fopen(sp, "r");									//Open file
    task * data_struct = malloc(sizeof(task));				//Allocate data
    if (reads==NULL) {										//If reading fails, return
      	perror("Error");
       	return;
   	}
    else
	{
	    while(!feof(reads))									//Iterate through file
		{
			fscanf(reads,"%d %d %d %d %d %d\n", &data_struct->deadline, &data_struct->period, &data_struct->release_time, &data_struct->priority, &data_struct->ID, &data_struct->quantum);
			waiting_queue=push(waiting_queue, *data_struct);	//Read file and push it to the waiting queue
        }
    }
    free(data_struct);										//Free the struct
}

//------------------Wake up task - moves tasks from waiting queue to ready queue------------------
void OS_wakeup_n()
{
	if (waiting_queue == NULL)			//If waiting queue is empty, return
	{
		return;
	}
	task * temp = waiting_queue;		//Handle to head of waiting queue
	while (temp != NULL)				//Iterate through waiting queue
	{
		if (OS_cycles >= temp->release_time)		//If the OS clock is greater or equal to the release time of a task, push it to the ready queue
		{
			ready_queue=push(ready_queue, *temp);				//Push the task
			waiting_queue=remove_node(waiting_queue, temp);		//Remove the task from the waiting queue
			temp = waiting_queue;								//Set our iterator to the new waiting queue
		}
		else
		{
			temp = temp->next;									//Iterate the list...
		}
	}
}

//------------------Scheduler, returns the task to be executed ------------------
task * scheduler_n()
{
	if (ready_queue != NULL)			//If the ready queue isn't empty, we have tasks ready to be returned from the scheduler
	{
		if (sched_type == sched_RR) 		//Here is the round robin (RR) scheduler, in the RR case, we just return the first element of the ready queue
		{
			return ready_queue;
		}
		if (sched_type == sched_SJF) 		//Here is where you implement your SJF scheduling algorithm
		{ //Shortest Job First - No Preemption (current executing job completes before picking next shortest job)
			sort(&ready_queue, sched_type); //sort the ready_queue in ascending order.
			return ready_queue;
		}
		if (sched_type == sched_MQ) 		//Here is where you implement your MQ scheduling algorithm,
		{
			while(ready_queue != NULL){//medans listan inte är tom(detta är för att tömma listan på alla tasks som inte är pågående)
				if(ready_queue->priority == 1){/*ta bort noden från ready queue, ändra prio och skicka nod till high*/
					ready_queue->priority = 8;//sätt till högsta prioritet
					high_queue = push(high_queue, *ready_queue);//skicka nod med ny prioritet till higgh_queue
					ready_queue = remove_front(ready_queue);
				}
				else if(ready_queue->priority == 8){/*ta bort noden från ready queue, ändra prio och skicka nod till medium*/
					ready_queue->priority--;
					medium_queue = push(medium_queue, *ready_queue);
					ready_queue = remove_front(ready_queue);
				}
				else if(ready_queue->priority == 7){/*låt nod stanna, sänk prioriteten och returnera denna nod*/
					ready_queue->priority--;
					return ready_queue;
				}
				else if(ready_queue->priority == 6){/*ta bort noden från ready queue, ändra prio och skicka nod till low*/
					ready_queue->priority--;
					low_queue = push(low_queue, *ready_queue);
					ready_queue = remove_front(ready_queue);
				}
				else if(ready_queue->priority == 5){/*låt nod stanna, sänk prioriteten och returnera denna nod*/
					ready_queue->priority--;
					return ready_queue;
				}
				else if(ready_queue->priority == 4){/*låt nod stanna, sänk prioriteten och returnera denna nod*/
					ready_queue->priority--;
					return ready_queue;
				}
				else if(ready_queue->priority == 3){/*låt nod stanna, sänk prioriteten och returnera denna nod*/
					ready_queue->priority--;
					return ready_queue;
				}
				else if(ready_queue->priority == 2){/*ta bort noden från ready queue, ändra prio och skicka nod till high*/
					ready_queue->priority = 8;
					high_queue = push(high_queue, *ready_queue);
					ready_queue = remove_front(ready_queue);
				}
				else{
					printf("ERROR");
					return NULL;
				}
			}//om loopen avslutas så är listan, ready_queue, tom

			if(high_queue != NULL){/*ta bort noden från ready queue och skicka nod till ready_queue och returnera nod*/
				ready_queue = push(ready_queue, *high_queue);
				high_queue = remove_front(high_queue);
				return ready_queue;
			}
			else if(medium_queue != NULL){/*ta bort noden från ready queue och skicka nod till ready_queue och returnera nod*/
				ready_queue = push(ready_queue, *medium_queue);
				medium_queue = remove_front(medium_queue);
				return ready_queue;
			}
			else if(low_queue != NULL){/*ta bort noden från ready queue och skicka nod till ready_queue och returnera nod*/
				ready_queue = push(ready_queue, *low_queue);
				low_queue = remove_front(low_queue);
				return ready_queue;
			}
			else{
				printf("ERROR");
				return NULL;
			}
		}

		if (sched_type == sched_EDF) 		//EDF scheduling algorithm
		{
			sort(&ready_queue, sched_type);
			return ready_queue;
		}
	}
	else						//If the ready queue is empty, the operating system must have something to do, therefore we return an idle task
	{
		idle_task->quantum++;			//Make sure that the idle task dosnt run out of quantum
		printf("RETURNED IDLE TASK");
		return idle_task;			//Return the idle task
	}
	return NULL;
}
//------------------ Dispatcher executes the task ------------------
void dispatch_n(task* exec)
{
	if (exec != exec_task) //If the pointer of the last executing task is not equal to the newly executing task a context switch has occurred
	{
		context_switches++; //Increase the context switches
		exec_task = exec; //Change the pointer of the currently executing task
	}

	exec->quantum--; //Decrease the time quantum of a task, i.e. run the task

	if (exec->quantum > 0)
	{
		if (exec == idle_task)
		{
			// Print
			printf("Idle task is executing - Total context switches: %d \n", context_switches);
		}
		else
		{
			// Print task info
			printf("Task %d is executing with %d quanta left - Total context switches: %d \n", exec->ID, exec->quantum, context_switches);
		}

		ready_queue = first_to_last(ready_queue); //Re-sort ready queue
	}
	else
	{
		printf("Task %d has executed and finished its quanta - Total context switches: %d \n", exec->ID, context_switches);	//Printout task info
		ready_queue->release_time = ready_queue->release_time+ready_queue->period;
		waiting_queue = push(waiting_queue, *ready_queue); 			//Add the finished task to the waiting queue
		ready_queue = pop(ready_queue); 						//Pop the finished task from ready queue
	}
}

void sched_picker(){ // user menu to avoid recompiling when showing different parts of the lab
	int user_input;
	printf("Choose scheduling policy: 1 = RR, 2 = SJF, 3 = MQ, 4 = EDF \n");
	scanf("%d",&user_input);
	if(user_input != 1 && user_input != 2 && user_input != 3 && user_input != 4){ // if other d input
		user_input = sched_RR; //Default round robin
	}
	sched_type = user_input; // scheduler_n() will pick correct scheduling algorithm.

	printf("Choose taskset: 1 = default, 2 = custom\n");
	scanf("%d", &user_input);
	if(user_input != 1 && user_input != 2){ // if other d input
		user_input = DEFAULT_TASKSET; //Default taskset.txt provided with lab.
	}
	task_set = user_input; // readTaskset_n() will load correct taskset.txt
}

int main(int argc, char **argv)
{
	sched_picker();			// user menu to avoid recompiling when demonstrating lab
	char * fp = "hej";			//File path to taskset
	readTaskset_n(fp); 			//Read taskset
	task * task_to_be_run; 		//Return taskset from scheduler
	idle_task = create(1337, 0, 0, 0, 0, 200000000, NULL);  //Create a dummy idle task
	while (1)
	{
		OS_wakeup_n();						//Wake up sleeping tasks
		task_to_be_run = scheduler_n();		//Fetch the task to be run
		dispatch_n(task_to_be_run);			//Dispatch the task to be run
		OS_cycles++;						//Increment OS clock
		usleep(1000000);					//Sleep so we dont get overflown with output
	}
}
