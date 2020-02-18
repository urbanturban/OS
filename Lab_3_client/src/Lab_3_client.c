/*
 ============================================================================
 Name        :	Lab_3_client.c
 Author      :	Akash Menon
 Version     :
 Copyright   : 	Your copyright notice
 Description : 	Client that creates planets from user input, and communicates
 	 	 	 	to a server through POSIX message queues.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "wrapper.h"

planet_type *my_planet;
pid_t pid;

int main(void){
	createPlanet();
	printPlanet();
}
void createPlanet() { /* Creates planet based on user input */
	my_planet = malloc(sizeof(planet_type));
		if(my_planet == NULL){
			printf("could not allocate memory for planet\n");
		}
	// User inputs the details of the planet
	printf("Planet NAME:\n");
	scanf("%s", &my_planet->name);
	printf("X-axis position:\n");
	scanf("%lf", &my_planet->sx);
	printf("Y-axis position:\n");
	scanf("%lf", &my_planet->sy);
	printf("X-axis velocity:\n");
	scanf("%lf", &my_planet->vx);
	printf("Y-axis velocity:\n");
	scanf("%lf", &my_planet->vy);
	printf("MASS:\n");
	scanf("%lf", &my_planet->mass);
	printf("LIFE:\n");
	scanf("%d", &my_planet->life);

	pid = getpid();
}
void printPlanet(){ /* Prints the user-defined details of the planet */
	printf("Name: %s\n", my_planet->name);
	printf("X: %.f\n", my_planet->sx);
	printf("Y: %.f\n", my_planet->sy);
	printf("Vx: %f\n", my_planet->vx);
	printf("Vy: %f\n", my_planet->vy);
	printf("Mass: %.f\n", my_planet->mass);
	printf("Life: %d\n", my_planet->life);
	printf("pid: %d\n", pid);
}
