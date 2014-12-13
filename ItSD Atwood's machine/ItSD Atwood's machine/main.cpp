//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
#include <iostream>
#include <ctime>
#define TIMER_START		clock_t t = clock()
#define TIMER_GET		(double)(clock() - t)
using namespace std;
#include <allegro5\allegro5.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>

#define TEXT_COLOR 255, 255, 255
#define TEXT_SIZE 17
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_FONT *font_big= NULL;
ALLEGRO_EVENT ev;

int winXsize = 800;
int winYsize = 800;
clock_t dt;
long recalc_counter = 0;

class machine
{
public:
	int  main_pulley_coord_x;
	int  main_pulley_coord_y;
	float radius;
	float mass_x1;
	float radius_2;
	float mass_x2, mass_x3;
	float left_length;
	float right_length;
	float left_minor_length;
	float right_minor_length;	
	machine(int x, int y, float r1, float m1, float r2, float m2, float m3, float l1, float l2, float lm1, float lm2)
	{
		main_pulley_coord_x = x;
		main_pulley_coord_y = y;
		radius = r1;
		mass_x1 = m1;
		radius_2 = r2;
		mass_x2 = m2;
		mass_x3 = m3;
		left_length = l1;
		right_length = l2;
		left_minor_length = lm1;
		right_minor_length = lm2;
	}
};


int allegro_initialization(int widht, int height)
{
	if (!al_init())
	{
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	else
		printf("Allegro initialized\n");

	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

	display = al_create_display(widht, height);
	if (!display)
	{
		al_show_native_message_box(display, "Error", "Error", "failed to create display!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	else
		printf("Display created\n");

	event_queue = al_create_event_queue();
	if (!event_queue)
	{
		al_show_native_message_box(display, "Error", "Error", "failed to create event_queue!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}
	else
		printf("Event queue created\n");
	al_flush_event_queue(event_queue);

	al_init_font_addon();
	al_init_ttf_addon();
	font = al_load_ttf_font("C:\\Windows\\Fonts\\cour.ttf", TEXT_SIZE, 0);
	font_big = al_load_ttf_font("C:\\Windows\\Fonts\\cour.ttf", 30, 0);
	if (!font || !font_big)
	{
		al_destroy_event_queue(event_queue);
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize font!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}
	else
		printf("Fonts installed\n");

	al_install_keyboard();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_init_primitives_addon();
	al_clear_to_color(al_map_rgb(0, 0, 20));
	al_flip_display();
	return 0;
}
long fib(int f)
{
	long ans;
	if (f <= 1)
		ans = 1;
	else if (f == 2)
		ans = 1;
	else
		ans = fib(f - 2) + fib(f - 1);
	//printf("fib=%d\n", ans);
	return ans;
}


int draw_on_screen(machine *atwood, clock_t current_time)
{
	#define RATIO_PX_per_CM 50
#define TOP_MARG (50+atwood->radius*RATIO_PX_per_CM)
	//define some colors
	ALLEGRO_COLOR wheel_color = al_map_rgb(100, 100, 100);
	ALLEGRO_COLOR mass_color = al_map_rgb(50, 50, 50);
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
	al_clear_to_color(al_map_rgb(0, 0, 20));
	//draw main pulley and its center
	al_draw_filled_circle(winXsize / 2, TOP_MARG, RATIO_PX_per_CM*atwood->radius, wheel_color);
	al_draw_filled_circle(winXsize / 2, TOP_MARG, 5, black);
	//draw mass 1 and its string
	al_draw_filled_rectangle(winXsize / 2 - atwood->radius*RATIO_PX_per_CM - (atwood->mass_x1 * 3), 
		TOP_MARG + atwood->left_length*RATIO_PX_per_CM, 
		winXsize / 2 - atwood->radius*RATIO_PX_per_CM + (atwood->mass_x1 * 3), 
		TOP_MARG + atwood->left_length*RATIO_PX_per_CM + atwood->mass_x1 * 7, 
		mass_color);
	al_draw_line(winXsize / 2 - atwood->radius*RATIO_PX_per_CM, 
		TOP_MARG, 
		winXsize / 2 - atwood->radius*RATIO_PX_per_CM, 
		TOP_MARG + atwood->left_length*RATIO_PX_per_CM, 
		white, 1);
	//draw pulley 2 , its center and its string
	al_draw_filled_circle(winXsize / 2 + atwood->radius*RATIO_PX_per_CM, 
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM, 
		atwood->radius_2*RATIO_PX_per_CM, 
		wheel_color);
	al_draw_filled_circle(winXsize / 2 + atwood->radius*RATIO_PX_per_CM, 
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM, 
		3, 
		black);
	al_draw_line(winXsize / 2 + atwood->radius*RATIO_PX_per_CM, 
		TOP_MARG, 
		winXsize / 2 + atwood->radius*RATIO_PX_per_CM, 
		TOP_MARG + atwood->left_length*RATIO_PX_per_CM, 
		white, 1);
	//draw left minor mass and its string
	al_draw_filled_rectangle(winXsize / 2 + atwood->radius*RATIO_PX_per_CM - atwood->radius_2*RATIO_PX_per_CM - atwood->mass_x2 * 3,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM + atwood->left_minor_length*RATIO_PX_per_CM,
		winXsize / 2 + atwood->radius*RATIO_PX_per_CM - atwood->radius_2*RATIO_PX_per_CM + atwood->mass_x2 * 3,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM + atwood->left_minor_length*RATIO_PX_per_CM + atwood->mass_x2 * 7,
		mass_color);
	al_draw_line(winXsize / 2 + atwood->radius*RATIO_PX_per_CM - atwood->radius_2*RATIO_PX_per_CM,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM,
		winXsize / 2 + atwood->radius*RATIO_PX_per_CM - atwood->radius_2*RATIO_PX_per_CM,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM + atwood->left_minor_length*RATIO_PX_per_CM,
		white, 1);
	//draw right minor mass and its string
	al_draw_filled_rectangle(winXsize / 2 + atwood->radius*RATIO_PX_per_CM + atwood->radius_2*RATIO_PX_per_CM - atwood->mass_x3 * 3,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM + atwood->right_minor_length*RATIO_PX_per_CM,
		winXsize / 2 + atwood->radius*RATIO_PX_per_CM + atwood->radius_2*RATIO_PX_per_CM + atwood->mass_x3 * 3,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM + atwood->right_minor_length*RATIO_PX_per_CM + atwood->mass_x3 * 7,
		mass_color);
	al_draw_line(winXsize / 2 + atwood->radius*RATIO_PX_per_CM + atwood->radius_2*RATIO_PX_per_CM,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM,
		winXsize / 2 + atwood->radius*RATIO_PX_per_CM + atwood->radius_2*RATIO_PX_per_CM,
		TOP_MARG + atwood->right_length*RATIO_PX_per_CM + atwood->right_minor_length*RATIO_PX_per_CM,
		white, 1);
	//describe left side l1
	al_draw_textf(font, white, winXsize / 2 - atwood->radius*RATIO_PX_per_CM - 70, TOP_MARG + atwood->radius*RATIO_PX_per_CM - TEXT_SIZE-2, 0, "l1=%.1f", atwood->left_length);
	//describe right side (l2, lm1, lm2)
	al_draw_textf(font, white, winXsize / 2 + atwood->radius*RATIO_PX_per_CM + 5, TOP_MARG + atwood->radius*RATIO_PX_per_CM - TEXT_SIZE-2, 0, "l2=%.1f", atwood->right_length);
	al_draw_textf(font, white, winXsize / 2 + atwood->radius*RATIO_PX_per_CM - atwood->radius_2*RATIO_PX_per_CM - 70, 
		TOP_MARG + atwood->radius_2*RATIO_PX_per_CM +atwood->right_length*RATIO_PX_per_CM - TEXT_SIZE - 2, 0, "l2=%.1f", atwood->left_minor_length);
	al_draw_textf(font, white, winXsize / 2 + atwood->radius*RATIO_PX_per_CM + atwood->radius_2*RATIO_PX_per_CM + 5,  
		TOP_MARG + atwood->radius_2*RATIO_PX_per_CM +atwood->right_length*RATIO_PX_per_CM - TEXT_SIZE - 2, 0, "l3=%.1f", atwood->right_minor_length);

#define FPS_SAMPLE_NUMBER 5
	al_draw_textf(font_big, al_map_rgb(255, 100, 100), 5, 5, 0, "t=%.3f", ((float)current_time)/CLOCKS_PER_SEC);
	recalc_counter++;
	//static int fps_averager[FPS_SAMPLE_NUMBER];
	//fps_averager[recalc_counter%5] = 1/
	//int fps = 0;
	//for (int i = 0; i < FPS_SAMPLE_NUMBER; i++)
	//{
	//	fps += fps_averager[i];
	//}
	int fps = (int)(1.0 / (((double)(current_time - dt)) / 1000));
	if (fps < 1000 && fps > 0)
		al_draw_textf(font, white, winXsize - 5, 5, ALLEGRO_ALIGN_RIGHT, "FPS: %10d", fps);
	dt = current_time;
	al_draw_textf(font, white, 5, 35+5,					0, "m1=%.1f", atwood->mass_x1);
	al_draw_textf(font, white, 5, 35+5+(TEXT_SIZE+5),	0, "m2=%.1f", atwood->mass_x2);
	al_draw_textf(font, white, 5, 35+5+2*(TEXT_SIZE+5), 0, "m3=%.1f", atwood->mass_x3);
	long uselessfibtable[100] = { 0 };
	for (int i = 0; i < 25; i++)
		uselessfibtable[i] = fib(i);

	al_flip_display(); 
	return 0;
}

int main()
{
	clock_t time = 0;
	allegro_initialization(winXsize, winYsize);
	//(int x, int y, float r1, float m1, float r2, float m2, float m3, float l1, float l2, float lm1, float lm2)
	machine atwoodMachine(winXsize / 2, 100, 2.0, 10, 1.0, 5.0, 6.0, 5.0, 5.0, 3.0, 2.5);

	TIMER_START;
	while (1)
	{
		if (!al_is_event_queue_empty(event_queue))
		{
			al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
				return 0;  //exit program
		}
		clock_t time_elapsed = TIMER_GET;
		draw_on_screen(&atwoodMachine, time_elapsed);
	}
	
	system("pause");
	return 0;
}