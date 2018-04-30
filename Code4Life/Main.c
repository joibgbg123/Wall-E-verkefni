/*******************************************************************************
* rc_project_template.c
*
* This is meant to be a skeleton program for robotics cape projects.
* Change this description and file name
*******************************************************************************/

// usefulincludes is a collection of common system includes for the lazy
// This is not necessary for roboticscape projects but here for convenience
#include <rc_usefulincludes.h>
// main roboticscape API header
#include <roboticscape.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h> // for atoi
#include <getopt.h>



// function declarations
void on_pause_pressed();
void on_pause_released();
void Drive();


/*******************************************************************************
* int main()
*
* This template main function contains these critical components
* - call to rc_initialize() at the beginning
* - main while loop that checks for EXITING condition
* - rc_cleanup() at the end
*******************************************************************************/
int main(){
	// always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

	// do your own initialization here
	printf("\nHello BeagleBone\n");
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);

	// done initializing so set state to RUNNING
	rc_set_state(RUNNING);

	// Kveikjum a hardware
	rc_enable_motors();
	rc_enable_servo_power_rail();



	// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){
		// handle other states
		if(rc_get_state()==RUNNING){
			// do things
			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);
			Drive();
		}
		else if(rc_get_state()==PAUSED){
			// do other things
			rc_set_led(GREEN, OFF);
			rc_set_led(RED, ON);
		}
		// always sleep at some point
		usleep(100000);
	}

	// exit cleanly
	rc_cleanup();
	return 0;
}

///Keyrslan
void Drive()
{

    double duty_start = 0.4;
    double duty1 = duty_start;
    double duty2 = -duty_start;

    /* Never mind that one then
    /// Motor definitions
    int motor_right = 2;
    int motor_left = 1;
    */

	/// Encoder definitions
	int encoder_right = 2;
	int encoder_left = 1;
	int E1 = 0;
	int E2 = 0;


system("stty raw");  /// No need for pressing 'enter' after every input.


//char input;
//do{
    char input = getchar();
    printf("> ");

    switch(input){
        case 'w':
            rc_set_motor(motor_left, duty1);
            rc_set_motor(motor_right, duty2);

            printf("Autobots! Roll out \n");
            printf("| E1 | E2 |\n");
            printf("| %i | %i |\n",-E1,E2);

            E1 = rc_get_encoder_pos(encoder_left);
            E2 = -rc_get_encoder_pos(encoder_right);

            if (E1 > E2 && duty2 < 0.8)
            {
                duty2=+0.025;
                rc_set_motor(motor_right, duty2);
            }
            else if (E1 < E2 && duty1 < 0.8)
            {
                duty1=+0.025;
                rc_set_motor(motor_left, duty1);
            }
            else if (E1 > E2)
            {
                duty1=-0.025;
                rc_set_motor(motor_left, duty1);
            }
            else if (E1 < E2)
            {
                duty2=-0.025;
                rc_set_motor(motor_right, duty2);
            }
            else {
                /// keep driving if they are the same
            }
            break;

        case 's':
            rc_set_motor(motor_left, -duty1);
            rc_set_motor(motor_right, -duty2);

            printf("Run Away!!! \n");
            break;

        case 'a':
            rc_set_motor(motor_left, -duty_start/2);
            rc_set_motor(motor_right, -duty_start);

            printf("vinstri beygja \n");
            break;

        case 'd':

            rc_set_motor(motor_left, duty_start);
            rc_set_motor(motor_right, duty_start/2);
            printf("haegri beygja \n");
            break;

        case 'f': ///STOPPA
            rc_set_motor(motor_left, 0.0);
            rc_set_motor(motor_right, 0.0);
            printf("Stopp an thess ad haetta akstri \n");
            break;

        case 'q': ///
            rc_set_state(PAUSED);
            rc_set_motor(motor_left, 0.0);
            rc_set_motor(motor_right, 0.0);
            rc_disable_servo_power_rail();

            printf("Settur í PAUSED MODE \n");
            break;

        case 'b':
            rc_send_servo_pulse_us(4,1500);
            rc_send_servo_pulse_us(8,1500);
            printf("position 0°\n");
            /// rc_usleep(1000000/frequency_hz);
            break;

        case 'n':
            rc_send_servo_pulse_us(4,2100);
            rc_send_servo_pulse_us(8,2100);
            printf("position 60°\n");
            break;



        default:
            printf("WASD til ad hreyfast \n");
            printf("f til ad stoppa en vera entha i running \n");
            printf("q til ad fara i PAUSED og haetta \n");
            break;

    }
//}while(rc_get_state()==RUNNING);
//rc_set_motor(1, 0.0);
//rc_set_motor(2, 0.0);

}


/*******************************************************************************
* void on_pause_released()
*
* Make the Pause button toggle between paused and running states.
*******************************************************************************/
void on_pause_released(){
	// toggle betewen paused and running modes
	if(rc_get_state()==RUNNING)		rc_set_state(PAUSED);
	else if(rc_get_state()==PAUSED)	rc_set_state(RUNNING);
	return;
}

/*******************************************************************************
* void on_pause_pressed()
*
* If the user holds the pause button for 2 seconds, set state to exiting which
* triggers the rest of the program to exit cleanly.
*******************************************************************************/
void on_pause_pressed(){
	int i=0;
	const int samples = 100;	// check for release 100 times in this period
	const int us_wait = 2000000; // 2 seconds

	// now keep checking to see if the button is still held down
	for(i=0;i<samples;i++){
		rc_usleep(us_wait/samples);
		if(rc_get_pause_button() == RELEASED) return;
	}
	printf("long press detected, shutting down\n");
	rc_set_state(EXITING);
	return;
}

