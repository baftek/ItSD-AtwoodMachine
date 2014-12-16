#include <iostream>
#define TIMER_START		double t = al_get_time()
#define TIMER_GET		(al_get_time() - t)
using namespace std;
#include <allegro5\allegro5.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>

#define TEXT_COLOR 255, 255, 255
#define TEXT_SIZE 18
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_FONT *font_big = NULL;
ALLEGRO_EVENT ev;

#define G_VALUE 9.81f
int winXsize = 800;
int winYsize = 750;
double dt;
double last_time;

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
	float change1, change2;
	double a1, d1, a2, d2;

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
		change1 = change2 = 0;
		a1 = 0.1, a2 = 0.1;
	}
	machine();
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


int draw_on_screen(machine *atwood, double current_time)
{
#define RATIO_PX_per_M 30
#define TOP_MARG (50+2*atwood->radius*RATIO_PX_per_M)
	//define some colors
	ALLEGRO_COLOR wheel_color = al_map_rgb(100, 100, 100);
	ALLEGRO_COLOR mass_color = al_map_rgb(50, 50, 50);
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
	al_clear_to_color(al_map_rgb(0, 0, 20));
	//draw main pulley and its center
	al_draw_filled_circle(winXsize / 2, TOP_MARG - atwood->radius*RATIO_PX_per_M, RATIO_PX_per_M*atwood->radius, wheel_color);
	al_draw_filled_circle(winXsize / 2, TOP_MARG - atwood->radius*RATIO_PX_per_M, 5, black);
	//draw mass 1 and its string
	al_draw_filled_rectangle(winXsize / 2 - atwood->radius*RATIO_PX_per_M - (atwood->mass_x1 * 3),
		TOP_MARG + (atwood->left_length + atwood->change1)*RATIO_PX_per_M,
		winXsize / 2 - atwood->radius*RATIO_PX_per_M + (atwood->mass_x1 * 3),
		TOP_MARG + (atwood->left_length + atwood->change1)*RATIO_PX_per_M + atwood->mass_x1 * 7,
		mass_color);
	al_draw_line(winXsize / 2 - atwood->radius*RATIO_PX_per_M,
		TOP_MARG - atwood->radius*RATIO_PX_per_M,
		winXsize / 2 - atwood->radius*RATIO_PX_per_M,
		TOP_MARG + (atwood->left_length + atwood->change1)*RATIO_PX_per_M,
		white, 1);
	//draw pulley 2 , its center and its string
	al_draw_filled_circle(winXsize / 2 + atwood->radius*RATIO_PX_per_M,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M,
		atwood->radius_2*RATIO_PX_per_M,
		wheel_color);
	al_draw_filled_circle(winXsize / 2 + atwood->radius*RATIO_PX_per_M,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M,
		3,
		black);
	al_draw_line(winXsize / 2 + atwood->radius*RATIO_PX_per_M,
		TOP_MARG - atwood->radius*RATIO_PX_per_M,
		winXsize / 2 + atwood->radius*RATIO_PX_per_M,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M,
		white, 1);

#define SMALL_MARG atwood->radius_2*RATIO_PX_per_M
	//draw left minor mass and its string
	al_draw_filled_rectangle(winXsize / 2 + atwood->radius*RATIO_PX_per_M - atwood->radius_2*RATIO_PX_per_M - atwood->mass_x2 * 3,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M + (atwood->left_minor_length + atwood->change2)*RATIO_PX_per_M + SMALL_MARG,
		winXsize / 2 + atwood->radius*RATIO_PX_per_M - atwood->radius_2*RATIO_PX_per_M + atwood->mass_x2 * 3,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M + (atwood->left_minor_length + atwood->change2)*RATIO_PX_per_M + atwood->mass_x2 * 7 + SMALL_MARG,
		mass_color);
	al_draw_line(winXsize / 2 + atwood->radius*RATIO_PX_per_M - atwood->radius_2*RATIO_PX_per_M,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M,
		winXsize / 2 + atwood->radius*RATIO_PX_per_M - atwood->radius_2*RATIO_PX_per_M,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M + (atwood->left_minor_length + atwood->change2)*RATIO_PX_per_M + SMALL_MARG,
		white, 1);
	//draw right minor mass and its string
	al_draw_filled_rectangle(winXsize / 2 + atwood->radius*RATIO_PX_per_M + atwood->radius_2*RATIO_PX_per_M - atwood->mass_x3 * 3,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M + (atwood->right_minor_length - atwood->change2)*RATIO_PX_per_M + SMALL_MARG,
		winXsize / 2 + atwood->radius*RATIO_PX_per_M + atwood->radius_2*RATIO_PX_per_M + atwood->mass_x3 * 3,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M + (atwood->right_minor_length - atwood->change2)*RATIO_PX_per_M + atwood->mass_x3 * 7 + SMALL_MARG,
		mass_color);
	al_draw_line(winXsize / 2 + atwood->radius*RATIO_PX_per_M + atwood->radius_2*RATIO_PX_per_M,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M,
		winXsize / 2 + atwood->radius*RATIO_PX_per_M + atwood->radius_2*RATIO_PX_per_M,
		TOP_MARG + (atwood->right_length - atwood->change1)*RATIO_PX_per_M + (atwood->right_minor_length - atwood->change2)*RATIO_PX_per_M + SMALL_MARG,
		white, 1);
	// draw small limiting lines
	for (int i = -0.5*atwood->radius*RATIO_PX_per_M; i < 2.5*atwood->radius*RATIO_PX_per_M; i += 3)
	if (i<0.5*atwood->radius*RATIO_PX_per_M || i>1.5*atwood->radius*RATIO_PX_per_M)
		al_draw_pixel(winXsize / 2 - atwood->radius*RATIO_PX_per_M + i, TOP_MARG, al_map_rgb(255, 255, 255));
	for (int i = -0.5*atwood->radius_2*RATIO_PX_per_M; i < 2.5*atwood->radius_2*RATIO_PX_per_M; i += 3)
	if (i<0.5*atwood->radius_2*RATIO_PX_per_M || i>1.5*atwood->radius_2*RATIO_PX_per_M)
		al_draw_pixel(winXsize / 2 + atwood->radius*RATIO_PX_per_M - atwood->radius_2*RATIO_PX_per_M + i, TOP_MARG + atwood->right_length*RATIO_PX_per_M + atwood->radius_2*RATIO_PX_per_M - atwood->change1*RATIO_PX_per_M, al_map_rgb(255, 255, 255));

	//describe left side l1
	al_draw_textf(font, white, winXsize / 2 - atwood->radius*RATIO_PX_per_M - 70, TOP_MARG - TEXT_SIZE - 2, 0, "l1=%.1f", (atwood->left_length + atwood->change1));
	//describe right side (l2, lm1, lm2)
	al_draw_textf(font, white, winXsize / 2 + atwood->radius*RATIO_PX_per_M + 5, TOP_MARG - TEXT_SIZE - 2, 0, "l2=%.1f", (atwood->right_length - atwood->change1));
	al_draw_textf(font, white, winXsize / 2 + atwood->radius*RATIO_PX_per_M - atwood->radius_2*RATIO_PX_per_M - 70,
		TOP_MARG + atwood->radius_2*RATIO_PX_per_M + (atwood->right_length - atwood->change1)*RATIO_PX_per_M - TEXT_SIZE - 2, 0, "l2=%.1f", (atwood->left_minor_length + atwood->change2));
	al_draw_textf(font, white, winXsize / 2 + atwood->radius*RATIO_PX_per_M + atwood->radius_2*RATIO_PX_per_M + 5,
		TOP_MARG + atwood->radius_2*RATIO_PX_per_M + (atwood->right_length - atwood->change1)*RATIO_PX_per_M - TEXT_SIZE - 2, 0, "l3=%.1f", (atwood->right_minor_length - atwood->change2));

	al_draw_textf(font_big, al_map_rgb(255, 100, 100), 5, 5, 0, "t=%.3f", (current_time));
	//#define FPS_SAMPLE_NUMBER 5
	//static long recalc_counter = 0;
	//recalc_counter++;

	//static int fps_averager[FPS_SAMPLE_NUMBER] = { 0 };
	//unsigned long fps = 0;
	//current_time = al_get_time();
	//fps_averager[recalc_counter % 5] = (int)(1.0 / (((current_time - dt)) / 1000));
	//for (int i = 0; i < FPS_SAMPLE_NUMBER; i++)
	//{
	//	fps += fps_averager[i];
	//}
	al_draw_textf(font, white, winXsize - 5, 5, ALLEGRO_ALIGN_RIGHT, "LPS: %3d", (int)(1.0 / (((current_time - dt)) / 1000)));
	//al_draw_textf(font, white, winXsize - 5, 5, ALLEGRO_ALIGN_RIGHT, "FPS: %3d", fps/FPS_SAMPLE_NUMBER);
	dt = current_time;
	al_draw_textf(font, white, 5, 35 + 5, 0, "m1=%.1f", atwood->mass_x1);
	al_draw_textf(font, white, 5, 35 + 5 + (TEXT_SIZE + 5), 0, "m2=%.1f", atwood->mass_x2);
	al_draw_textf(font, white, 5, 35 + 5 + 2 * (TEXT_SIZE + 5), 0, "m3=%.1f", atwood->mass_x3);
	al_draw_textf(font, white, 5, 35 + 5 + 3 * (TEXT_SIZE + 5), 0, "a1=%.4f v1=%.4f", atwood->a1, atwood->a1*current_time);
	al_draw_textf(font, white, 5, 35 + 5 + 4 * (TEXT_SIZE + 5), 0, "a2=%.4f v2=%.4f", atwood->a2, atwood->a2*current_time);
	al_draw_textf(font, white, 5, 35 + 5 + 5 * (TEXT_SIZE + 5), 0, "T=%f", 2 * G_VALUE*atwood->mass_x1*(atwood->mass_x2 + atwood->mass_x3) / (atwood->mass_x1 + (atwood->mass_x2 + atwood->mass_x3)));

	//test = movable diameter
#define M_PI       3.14159265358979323846
#define COSSIN_CONTENT ((atwood->change1)/(2*M_PI*atwood->radius)*2*M_PI)
	al_draw_line(winXsize / 2 + sin(COSSIN_CONTENT)*atwood->radius*RATIO_PX_per_M,
		TOP_MARG - atwood->radius*RATIO_PX_per_M + cos(COSSIN_CONTENT)*atwood->radius*RATIO_PX_per_M,
		winXsize / 2 - sin(COSSIN_CONTENT)*atwood->radius*RATIO_PX_per_M,
		TOP_MARG - atwood->radius*RATIO_PX_per_M - cos(COSSIN_CONTENT)*atwood->radius*RATIO_PX_per_M,
		al_map_rgb(255, 100, 100), 1.5);

	//long uselessfibtable[100] = { 0 };
	//for (int i = 0; i < 25; i++)
	//	uselessfibtable[i] = fib(i);

	al_flip_display();
	return 0;
}

int recalculate_machine(machine *atwood, double current_time)
{
	int i = 2;
	// x=1/2 at^2
	// for main pulley
	if (atwood->left_length + atwood->change1 > 0 && atwood->right_length - atwood->change1 > 0 && atwood->a1 != 0)
	{
		double m2 = atwood->mass_x2 + atwood->mass_x3;
		double m1 = atwood->mass_x1;
		//assuming m1 > m2
		atwood->a1 = G_VALUE*(m1 - m2) / (m1 + m2);
		// displacement
		atwood->change1 = 0.5*atwood->a1*pow(((current_time)), 2);
	}
	else
		i -= 1;
	if (atwood->right_minor_length - atwood->change2 > 0 && atwood->left_minor_length + atwood->change2 > 0 && atwood->a2 != 0)
	{
		//assuming mm1 > mm2
		atwood->a2 = G_VALUE*(atwood->mass_x2 - atwood->mass_x3) / (atwood->mass_x2 + atwood->mass_x3);
		// displacement
		atwood->change2 = 0.5 * atwood->a2 * pow(((current_time)), 2);
	}
	else
		i -= 1;
	//printf("a=%f\tt=%f\td1=%f\td2=%f\t\t%f\n", atwood->a1, (current_time, atwood->change1, atwood->change2, atwood->left_length));
	last_time = current_time;
	return i;
}

void printhelp()
{
	cout << "Possible imput combinations:" << endl;
	cout << "itsd.exe m1 m2 m3" << endl;
	cout << "itsd.exe m1 m2 m3 r1 r2" << endl;
	cout << "itsd.exe m1 m2 m3 l1 l2 l3 l4" << endl;
	cout << "itsd.exe m1 m2 m3 l1 l2 l3 l4 r1 r2" << endl;
	cout << "where: \n\tm=mass \n\tl=lenght of wire \n\tr=radius of pulley" << endl;
	return;
}

int main(int argc, char **argv)
{
	double time = 0;
	bool repeat = false;
	machine *atwoodMachine = NULL;
	do{
		allegro_initialization(winXsize, winYsize);
		//(int x, int y, float r1, float m1, float r2, float m2, float m3, float l1, float l2, float lm1, float lm2)
		if (argc == 2 && strstr(argv[1], "h"))
		{
			printhelp();
			return 0;
		}
		else if (argc==1)
			atwoodMachine = new machine(winXsize / 2, 100, 2.0, 10.9, 1.0, 5.0, 6.0, 5.0, 5.0, 3.0, 2.5);
		else if (argc == 4)	//masses only
			atwoodMachine = new machine(winXsize / 2, 100, 2.0, atof(argv[1]), 1.0, atof(argv[2]), atof(argv[3]), 5.0, 5.0, 3.0, 2.5);
		else if (argc == 6)	//masses and radiuses
			atwoodMachine = new machine(winXsize / 2, 100, atof(argv[4]), atof(argv[1]), atof(argv[5]), atof(argv[2]), atof(argv[3]), 5.0, 5.0, 3.0, 2.5);
		else if (argc == 8)
			atwoodMachine = new machine(winXsize / 2, 100, 2.0, atof(argv[1]), 1.0, atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]));
		else if (argc == 10)
			atwoodMachine = new machine(winXsize / 2, 100, atof(argv[8]), atof(argv[1]), atof(argv[9]), atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]));
		draw_on_screen(atwoodMachine, 0);
		al_draw_text(font_big, al_map_rgb(255, 255, 255), winXsize / 2, 50, 1, "Press any key to start");
		al_draw_text(font_big, al_map_rgb(255, 255, 255), winXsize / 2 + 1, 50, 1, "Press any key to start");
		al_flip_display();
		while (1)
		{
			if (!al_is_event_queue_empty(event_queue))
			{
				al_wait_for_event(event_queue, &ev);
				if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
					return 0;  //exit program
				else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
					break;
			}
		}

		TIMER_START;
		while (1)
		{
			if (!al_is_event_queue_empty(event_queue))
			{
				al_wait_for_event(event_queue, &ev);
				if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
					return 0;  //exit program
			}
			double time_elapsed = TIMER_GET;
			if (!recalculate_machine(atwoodMachine, time_elapsed))	// if everything stopped => stop simulation
				break;
			draw_on_screen(atwoodMachine, time_elapsed);
		}
		//now: simulation ended, waiting for Q or R
		while (1)
		{
			if (!al_is_event_queue_empty(event_queue))
			{
				al_wait_for_event(event_queue, &ev);
				if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
					return 0;  //exit program
				else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_R))
				{
					repeat = true;
					al_destroy_display(display);
					break;
				}
			}
		}
	} while (repeat);

	system("pause");
	return 0;
}