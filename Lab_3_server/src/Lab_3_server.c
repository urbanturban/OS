#include <cairo.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <semaphore.h>
#include "wrapper.h"

#define SERVER_MQ "/superQueue3451"

#define DT 10
static void do_drawing(cairo_t *);
int x = 0;
int y = 0;
int x2 = 0;
GtkWidget *window;
GtkWidget *darea;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
planet_type * planet_list = NULL; //head
void calculate_planet_pos(planet_type *p1);
void add_Planet(planet_type *p);
void delete_Planet(planet_type *p);

void * planet_thread (void*args) //calculates own position every 10ms
{
	planet_type this_planet = *((planet_type *)args);
	pthread_mutex_lock(&mutex);
	add_Planet(&this_planet);
	pthread_mutex_unlock(&mutex);

	//mq Hantering
	char mqToClientName[30]="\0";
	strcpy(mqToClientName, this_planet.pid);
	mqd_t mqToClient;
	MQconnect(&mqToClient, mqToClientName);
	//mq Hantering

	while(this_planet.life > 0 && //as long as planet has life
			(this_planet.sx > 0) && (this_planet.sx < 800)) //&& //is within bounds: x axis
			//(this_planet.sy > 0) && (this_planet.sy < 800)) //is withing bounds: y axis
	{
		usleep(10000);
		pthread_mutex_lock(&mutex);
		calculate_planet_pos(&this_planet);
		pthread_mutex_unlock(&mutex);
	}


	//MQ hantering
	//TODO death reason
	MQwrite(mqToClient, &this_planet);
	usleep(10);
	MQclose(&mqToClient, mqToClientName);
	//MQ hantering

	pthread_mutex_lock(&mutex);
	delete_Planet(&this_planet);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
	//TODO PRINT MESSAGE TO MQ, THAT LIFE HAS ENDED.
}

void add_Planet(planet_type* planet_to_add){
	if(planet_list == NULL){ //if its the first planet added
		planet_list = planet_to_add;
	}
	else{
		planet_type * temp = planet_list;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = planet_to_add;
	}
}

void delete_Planet(planet_type* planet_to_delete){
	if(planet_list == NULL){
		//printf("Nothing to delete\n");
	}
	else {
		planet_type *temp = planet_list;
		while(temp->next != planet_to_delete){
			temp = temp->next;
		}
		temp->next = planet_to_delete->next;

	}
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, //Draw event for cairo, will be triggered each time a draw event is executed
                              gpointer user_data)
{
    do_drawing(cr); //Launch the actual draw method
    return FALSE; //Return something
}

static void do_drawing(cairo_t *cr) //Do the drawing against the cairo surface area cr
{
    //------------------------------------------Insert planet drawings below-------------------------------------------
	cairo_set_source_rgb(cr, 0, 0, 0); //Set RGB source of cairo, 0,0,0 = black
    cairo_select_font_face(cr, "Purisa",
          CAIRO_FONT_SLANT_NORMAL,
          CAIRO_FONT_WEIGHT_BOLD);

    	planet_type *planet_to_draw;
    //pthread_mutex_lock(&mutex);
    	planet_to_draw = planet_list;
    	while(planet_to_draw != NULL){
			if(strcmp(planet_to_draw->name, "Sun") == 0){
				cairo_set_source_rgb(cr, 1, 0, 0); //Set RGB source of cairo, 0,0,0 = black
			}
			else if(strcmp(planet_to_draw->name, "Earth") == 0) {
				cairo_show_text(cr, "PRINTING EARTH");
				cairo_set_source_rgb(cr, 0, 0, 1);
			}
			else if(strcmp(planet_to_draw->name, "Comet") == 0) {
				cairo_set_source_rgb(cr, 0, 1, 0);
			}
			else if(strcmp(planet_to_draw->name, "Dying star") == 0) {
				cairo_set_source_rgb(cr, 0.5, 0.2, 0.1);
			}
			else {
				cairo_set_source_rgb(cr, 0, 0, 0);
			}
			cairo_arc(cr, planet_to_draw->sx,planet_to_draw->sy,25,0,2*3.1415); //These drawings are just examples, remove them once you understood how to draw your planets
			cairo_fill(cr);
			cairo_move_to(cr, planet_to_draw->sx-50,planet_to_draw->sy-30);
		    cairo_show_text(cr, planet_to_draw->name);

		    int length = snprintf( NULL, 0, "%d", planet_to_draw->life);			//Converting life to printable string
			char* life = malloc( length + 1 );										//Converting life to printable string
			snprintf( life, length + 1, "%d", planet_to_draw->life);				//Converting life to printable string

		    cairo_move_to(cr, planet_to_draw->sx+50,planet_to_draw->sy+30);
		    cairo_show_text(cr, life);
		    free(life);
		    planet_to_draw = planet_to_draw->next;
    	}
    //pthread_mutex_unlock(&mutex);


    //------------------------------------------Insert planet drawings Above-------------------------------------------

}
GtkTickCallback on_frame_tick(GtkWidget * widget, GdkFrameClock * frame_clock, gpointer user_data) //Tick handler to update the frame
{
    gdk_frame_clock_begin_updating (frame_clock); //Update the frame clock
    gtk_widget_queue_draw(darea); //Queue a draw event
    gdk_frame_clock_end_updating (frame_clock); //Stop updating frame clock
}

void calculate_planet_pos(planet_type *p1)  //Function for calculating the position of a planet, relative to all other planets in the system
{
    planet_type *current = planet_list; //Pointer to head in the linked list

    //Variable declarations
    double Atotx = 0;
    double Atoty = 0;
    double x = 0;
    double y = 0;
    double r = 0;
    double a = 0;
    double ax = 0;
    double ay = 0;

    double G = 6.67259 * pow(10, -11); //Declaration of the gravitational constant
    while (current != NULL) //Loop through all planets in the list
    {
        if (p1 != current) //Only update variables according to properties of other planets
        {
            x = current->sx - p1->sx;
            y = current->sy - p1->sy;
            r = sqrt(pow(x, 2.0) + pow(y, 2.0));
            a = G * (current->mass / pow(r, 2.0));

            ay = a * (y / r);
            ax = a * (x / r);

            Atotx += ax;
            Atoty += ay;

        }
        current = current->next;
    }
    p1->vx = p1->vx + (Atotx * DT); //Update planet velocity, acceleration and life
    p1->vy = p1->vy + (Atoty * DT);
    p1->sx = p1->sx + (p1->vx * DT);
    p1->sy = p1->sy + (p1->vy * DT);
    p1->life -= 1;
}

void * MQ_listener(void * args){
	pthread_t pt;
	mqd_t serverMQ;
	char MQserverName[] = SERVER_MQ;
	MQcreate(&serverMQ, MQserverName);

	planet_type planet = {0};
	planet_type* ptr = &planet;

	while(strcmp(planet.name, "deathstar") != 0){
		MQread(serverMQ, &ptr);
		if(planet.name != NULL && strcmp(planet.name, "deathstar") != 0){
			pthread_create(&pt, NULL, &planet_thread, &planet);
			usleep(10);
			strcpy(planet.name, "\0");
		}
	}

	MQclose(&serverMQ, MQserverName);
	mq_unlink(MQserverName);
}

int main(int argc, char *argv[]) //Main function
{
    //----------------------------------------Variable declarations should be placed below---------------------------------
	pthread_t i_am_thread;
	pthread_t i_am_thread2;
	pthread_t i_am_thread3;
	//planet_list = (planet_type*)malloc(sizeof(planet_type));

    //----------------------------------------Variable declarations should be placed Above---------------------------------

    //GUI stuff, don't touch unless you know what you are doing, or if you talked to me
    gtk_init(&argc, &argv); //Initialize GTK environment
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //Create a new MQ_listenerwindow which will serve as your top layer
    darea = gtk_drawing_area_new(); //Create draw area, which will be used under top layer window
    gtk_container_add(GTK_CONTAINER(window), darea); //add draw area to top layer window
    g_signal_connect(G_OBJECT(darea), "draw",
                     G_CALLBACK(on_draw_event), NULL); //Connect callback function for the draw event of darea
    g_signal_connect(window, "destroy", //Destroy event, not implemented yet, altough not needed
                     G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); //Set position of window
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600); //Set size of window
    gtk_window_set_title(GTK_WINDOW(window), "GTK window"); //Title
    gtk_widget_show_all(window); //Show window
    gtk_widget_add_tick_callback(darea, on_frame_tick, NULL, 1); //Add timer callback functionality for darea
    //GUI stuff, don't touch unless you know what you are doing, or if you talked to me


    //-------------------------------Insert code for pthreads below------------------------------------------------
    pthread_create(&i_am_thread, NULL, &MQ_listener, NULL);//Create MQ_listener thread
    //Create MQ_listener thread
    //pthread_create(&i_am_thread3, NULL,&planet_thread,&testPlanet);
    //pthread_create(&i_am_thread2, NULL,&planet_thread,&testPlanet2);

    //-------------------------------Insert code for pthreads above------------------------------------------------


    gtk_main();//Call gtk_main which handles basic GUI functionality
    return 0;
}

