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

#define SERVER_MQ "/superQueue3451"

int numberOfPlanets = 1;

void * deathMessage(void*args){

	mqd_t mqFromServer;
	char mqFromSeName[30];
	strcpy( mqFromSeName,(char*)args);//kopiera rätt mq namn över i mqFromSeName
	MQcreate(&mqFromServer, mqFromSeName);

	planet_type buffer;
	planet_type *ptr = &buffer;

	while(numberOfPlanets > 0){
		MQread(mqFromServer, &ptr);
		if(strcmp(buffer.name, "\0") != 0){
			printf("%s is dead\n", buffer.name);
			strcpy( buffer.name, "\0");
			numberOfPlanets--;
		}
		sleep(1);
	}

	MQclose(&mqFromServer, mqFromSeName);
	mq_unlink(mqFromSeName);

}

int main(void)
{
	int onlyTest = 0;

	pthread_t dM;

	planet_type planet={0};
	char mqFromSeName[30] = "\0";
	strcpy(planet.pid, "/\0");
	pid_t pid = getpid();
	sprintf(mqFromSeName, "%d", (int)pid);//skriv över PID till mqFromSeName
	strcat(planet.pid, mqFromSeName);//lägg till PID till planet PID som då består av "/"+PID
	strcpy(mqFromSeName, planet.pid);//kopiera så att mqFromSeName  har samma som planet.pid

	pthread_create(&dM, NULL, &deathMessage, mqFromSeName);


	char toServerMQName[] = SERVER_MQ;
	mqd_t mqToServer;
	int status = MQconnect(&mqToServer, toServerMQName);
	if(status != 1){
		printf("ERROR! COULD NOT CONNECT TO SERVER_MQ");
		//return-1;
	}

	if(onlyTest == 0){

		planet_type testPlanet = {0};
		strcpy(testPlanet.name,"Earth\0");	// Name of planet
		testPlanet.sx = 200;			// X-axis position
		testPlanet.sy = 300;			// Y-axis position
		testPlanet.vx = 0;			// X-axis velocity
		testPlanet.vy = 0.008;			// Y-axis velocity
		testPlanet.mass = 1000;		// Planet mass
		testPlanet.next = NULL;		// Pointer to next planet in linked list
		testPlanet.life = pow(10,8);		// Planet life
		strcpy(testPlanet.pid, mqFromSeName);	// String containing ID of creating process

		planet_type testPlanet2 = {0};
		strcpy(testPlanet2.name,"Sun\0");	// Name of planet
		testPlanet2.sx = 300;			// X-axis position
		testPlanet2.sy = 300;			// Y-axis position
		testPlanet2.vx = 0;			// X-axis velocity
		testPlanet2.vy = 0;			// Y-axis velocity
		testPlanet2.mass = pow(10,8);		// Planet mass
		testPlanet2.next = NULL;		// Pointer to next planet in linked list
		testPlanet2.life = pow(10,8);		// Planet life
		strcpy(testPlanet2.pid, mqFromSeName);

		planet_type planet3 = {0};
		strcpy(planet3.name,"comet\0");	// Name of planet
		planet3.sx = 500;			// X-axis position
		planet3.sy = 300;			// Y-axis position
		planet3.vx = 0.1;			// X-axis velocity
		planet3.vy = 0;			// Y-axis velocity
		planet3.mass = pow(10,3);		// Planet mass
		planet3.next = NULL;		// Pointer to next planet in linked list
		planet3.life = pow(10,8);		// Planet life
		strcpy(planet3.pid, mqFromSeName);

		planet_type dyingstar = {0};
		strcpy(dyingstar.name,"Dying star\0");	// Name of planet
		dyingstar.sx = 500;			// X-axis position
		dyingstar.sy = 400;			// Y-axis position
		dyingstar.vx = 0;			// X-axis velocity
		dyingstar.vy = 0;			// Y-axis velocity
		dyingstar.mass = pow(10,9);		// Planet mass
		dyingstar.next = NULL;		// Pointer to next planet in linked list
		dyingstar.life = pow(10,3);		// Planet life
		strcpy(dyingstar.pid, mqFromSeName);

		MQwrite(mqToServer, &testPlanet);
		MQwrite(mqToServer, &testPlanet2);
		MQwrite(mqToServer, &planet3);
		MQwrite(mqToServer, &dyingstar);
		numberOfPlanets = 4;

		sleep(20);

		planet_type deathstar;
		strcpy(deathstar.name, "deathstar");
		MQwrite(mqToServer, &deathstar);

	}
	else{
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

				//printf("\n%s:\nMass: %lf\nX-axis position: %lf\nY-axis position: %lf\nX-axis velocity: %lf\nY-axis velocity: %lf\nLife: %d\n", planet.name, planet.mass, planet.sx, planet.sy, planet.vx, planet.vy, planet.life);

				MQwrite(mqToServer, &planet);
				numberOfPlanets++;
			}
		}
	}


	MQclose(&mqToServer, toServerMQName);

	printf("END");
	pthread_exit(NULL);
	//return EXIT_SUCCESS;
}
