/*
 ============================================================================
 Name        : 	Lab_3_client.c
 Author      : 	Edvin Asmussen, Akash Menon
 Version     :
 Copyright   : 	Your copyright notice
 Description : 	Client, that makes planets based on user input,
 	 	 	 	sends them over POSIX MQ to server.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "wrapper.h"

#define SERVER_MQ "/superQueue"

int main(void)
{
	//printf("Hello lab 3!\n");

	char toServerMQName[] = SERVER_MQ;

	planet_type planet;
	pid_t pid = getpid();
	sprintf(planet.pid, "%d", (int)pid);
	mqd_t mqFromServer = NULL;

	//char mqFromSeName* = (char*)malloc(sizeof(pid));
	//char mqFromSeName[] = "/";


	mqd_t mqToServer;
	if(MQconnect(&mqToServer, toServerMQName) != 1){
		printf("ERROR! COULD NOT CONNECT TO SERVER_MQ");
		//return-1;
	}

	int menu = 0;

	while(menu != -1){
		printf("New planet? 1(yes) or -1(ENDs program)\n");
		scanf("%d", &menu);

		if(menu == 1){

			printf("Planet name:\n");
			scanf("%s", planet.name);
			printf("%s mass: ", planet.name);
			scanf("%lf", &planet.mass);
			printf("%s X-axis position: ", planet.name);
			scanf("%lf", &planet.sx);
			printf("%s Y-axis position: ", planet.name);
			scanf("%lf", &planet.sy);
			printf("%s X-axis velocity: ", planet.name);
			scanf("%lf", &planet.vx);
			printf("%s Y-axis velocity: ", planet.name);
			scanf("%lf", &planet.vy);
			printf("%s life: ", planet.name);
			scanf("%d", &planet.life);

			printf("\n%s:\nMass: %lf\nX-axis position: %lf\nY-axis position: %lf\nX-axis velocity: %lf\nY-axis velocity: %lf\nLife: %d\n", planet.name, planet.mass, planet.sx, planet.sy, planet.vx, planet.vy, planet.life);

			if(mqFromServer == NULL){
				usleep(10);
				//MQcreate(&mqFromServer, mqFromSeName);
			}

			MQwrite(mqToServer, &planet);

		}
		else {}
	}


	MQclose(&mqToServer, toServerMQName);
	//MQclose(&mqFromServer);
	mq_unlink(mqFromServer);

	printf("END");
	return EXIT_SUCCESS;
}
