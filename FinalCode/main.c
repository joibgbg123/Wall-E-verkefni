#include <stdio.h>
#include <stdlib.h>

#include <rc_usefulincludes.h>
#include <roboticscape.h>

// function declarations
void Drive();
void on_pause_released();
void EncoderTest(int EncoderLeft, int EncoderRight, double duty_start);

int main()
{
	// always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

	printf("\nDestroy all humans\n");

	///All definitions & functions here below
	rc_set_pause_released_func(&on_pause_released);
	rc_set_state(RUNNING);

	/// Activate Hardware
	rc_enable_motors();
	rc_enable_servo_power_rail();

    /// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){

		if(rc_get_state()==RUNNING){
			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);

			Drive();
		}

		else if(rc_get_state()==PAUSED){

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

void Drive()
{
    /// Motor definitions
    int motor_right = 2;
    int motor_left = 1;

    double duty_start = 0.4;
    double dutyLeft = duty_start;
    double dutyRight = -duty_start;

	/// Encoder definitions
	int encoder_right = 2;
	int encoder_left = 1;
    int EncoderLeft = rc_get_encoder_pos(encoder_left);
    int EncoderRight = -rc_get_encoder_pos(encoder_right);

    system("stty raw");  /// No need for pressing 'enter' after every input.

    char input = getchar(); //user input
    printf("> ");

    switch(input){
        case 'w':
            printf("| BAHAn EncoderLeft | EncoderRight |\n");
            printf("| %i | %i |\n",EncoderLeft,EncoderRight);

            rc_set_motor(motor_left, dutyLeft);
            rc_set_motor(motor_right, dutyRight);

            printf("Autobots! Roll out \n");

            EncoderTest(EncoderLeft, EncoderRight, duty_start);
            break;

        case 's':
            rc_set_motor(motor_left, -dutyLeft);
            rc_set_motor(motor_right, -dutyRight);
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
            printf("Paused mode on \n");
            break;

        case 'q': /// Haetta
            rc_set_state(EXITING);
            rc_set_motor(motor_left, 0.0);
            rc_set_motor(motor_right, 0.0);
            printf("Exiting program \n");
            break;

        default:
            printf("WASD til ad hreyfast \n");
            printf("f til ad stoppa en vera entha i running \n");
            printf("q til ad fara i PAUSED og haetta \n");
            break;

    }
}

void EncoderTest(int EncoderLeft, int EncoderRight, double duty_start)
{
    /// Motor definitions
    int motor_right = 2;
    int motor_left = 1;

    double dutyLeft = duty_start;
    double dutyRight = -duty_start;

	/// Encoder definitions
	int encoder_right = 2;
	int encoder_left = 1;

    while(EncoderLeft != EncoderRight){
        printf("| EncoderLeft | EncoderRight |\n");
        printf("| %i | %i |\n",EncoderLeft,EncoderRight);

        if(EncoderLeft < EncoderRight){
            rc_set_motor(motor_left, dutyLeft - 0.2);
            rc_set_motor(motor_right, dutyRight - 0.2);
        }

        if(EncoderLeft > EncoderRight){
            rc_set_motor(motor_left, dutyLeft + 0.2);
            rc_set_motor(motor_right, dutyRight + 0.2);
        }

        EncoderLeft = rc_get_encoder_pos(encoder_left);
        EncoderRight = -rc_get_encoder_pos(encoder_right);
    }
    rc_set_motor(motor_left, dutyLeft);
    rc_set_motor(motor_right, dutyRight);

}

void on_pause_released(){
	// toggle betewen paused and running modes
	if(rc_get_state()==RUNNING)		rc_set_state(PAUSED);
	else if(rc_get_state()==PAUSED)	rc_set_state(RUNNING);
	return;
}
