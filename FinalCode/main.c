#include <stdio.h>
#include <stdlib.h>

#include <rc_usefulincludes.h>
#include <roboticscape.h>

#define BASE_DUTY 0.4
#define ADJUSTMENT 0.01

static volatile int Thread_switch = 1;
static volatile char Turning_switch = 'w';

// function declarations
void Drive();
void on_pause_released();
void *encoderEntry(void *param);
pthread_mutex_t lock;

int main()
{
	// always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

	printf("\nDestroy all humans\n\r");

	///All definitions & functions here below
	rc_set_pause_released_func(&on_pause_released);
	rc_set_state(RUNNING);

	/// Activate Hardware
	rc_enable_motors();
	rc_enable_servo_power_rail();

    /// ENABLE all Threads
	pthread_t encoderThread; /// HERE2 !!!!!!!!!
	pthread_create(&encoderThread, NULL, encoderEntry, NULL); /// HERE3 !!!!!!!!

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

    ///Clean the Thread
	//pthread_join(encoderThread, NULL); /// HERE4 !!!
	int pthread_cancel(pthread_t encoderThread);

	// exit cleanly
	rc_cleanup();
    return 0;
}

void Drive()
{
    /// Motor definitions
    int motor_right = 2;
    int motor_left = 1;

    double dutyLeft = BASE_DUTY;
    double dutyRight = -BASE_DUTY;

	/// Encoder definitions
	int encoder_right = 2;
	int encoder_left = 1;
    int EncoderLeft = rc_get_encoder_pos(encoder_left);
    int EncoderRight = -rc_get_encoder_pos(encoder_right);

    system("stty raw");  /// No need for pressing 'enter' after every input.

    char input = getchar(); //user input
    printf("> ");

    pthread_mutex_lock(&lock);
    switch(input){
        case 'w':
            Turning_switch = 'w';
            printf("| driveF: EncoderLeft | EncoderRight |\n\r");
            printf("| %i | %i |\n\r",EncoderLeft,EncoderRight);

            rc_set_motor(motor_left, dutyLeft);
            rc_set_motor(motor_right, dutyRight);
            printf("Autobots! Roll out \n\r");
            break;

        case 's':
            Turning_switch = 'w'; //Thread
            rc_set_motor(motor_left, -dutyLeft);
            rc_set_motor(motor_right, -dutyRight);
            printf("Run Away!!! \n\r");
            break;

        case 'a':
            Turning_switch = 'a'; //Thread
            rc_set_motor(motor_left, -dutyLeft/2);
            rc_set_motor(motor_right, -dutyRight);
            printf("vinstri beygja \n\r");
            break;

        case 'd':
            Turning_switch = 'a'; //Thread
            rc_set_motor(motor_left, dutyLeft);
            rc_set_motor(motor_right, dutyRight/2);
            printf("haegri beygja \n\r");
            break;

        case 'f': ///STOPPA
            Turning_switch = 'f'; //Thread
            rc_set_motor(motor_left, 0.0);
            rc_set_motor(motor_right, 0.0);
            printf("Paused mode on \n\r");
            break;

        case 'q': /// Haetta
            Thread_switch = 0; //Thread
            rc_set_state(EXITING);
            rc_set_motor(motor_left, 0.0);
            rc_set_motor(motor_right, 0.0);
            printf("Exiting program \n\r");
            break;

        default:
            printf("WASD til ad hreyfast \n\r");
            printf("f til ad stoppa en vera entha i running \n\r");
            printf("q til ad fara i PAUSED og haetta \n\r");
            break;

    }
    pthread_mutex_unlock(&lock);
}

void *encoderEntry(void *param)
{
    /// Motor definitions
    int motor_right = 2;
    int motor_left = 1;

    double dutyLeft = BASE_DUTY;
    double dutyRight = -BASE_DUTY;

    while(Thread_switch == 1){

        /// Encoder definitions
        int EncoderLeft = rc_get_encoder_pos(motor_left);
        int EncoderRight = -rc_get_encoder_pos(motor_right);

        if(Turning_switch == 'w'){
            printf("| Left | Right |\n\r");
            printf("|  %i  |  %i  |\n\r",EncoderLeft,EncoderRight);
            printf("|  %f  |  %f  |\n\r",dutyLeft,dutyRight);

            if(EncoderLeft < EncoderRight){
                rc_set_motor(motor_right, dutyRight += ADJUSTMENT);
                printf("+0.01 Left\n\r");
            }
            else if(EncoderLeft > EncoderRight){
                rc_set_motor(motor_right, dutyRight -= ADJUSTMENT);
                printf("-0.01 Right\n\r");
            }
        }

        else if(Turning_switch == 'a'){
            rc_set_encoder_pos(motor_left, 0);
            rc_set_encoder_pos(motor_right, 0);

            printf("| Left | Right |\n\r");
            printf("|  %i  |  %i  |\n\r",EncoderLeft,EncoderRight);
        }

        else if(Turning_switch == 'f'){
            usleep(10000000); /// wait for 10 second
            printf("Im on a break\n\r");
        }


        rc_usleep(800000); /// wait for 0.8 second
    }

    return NULL;
}

void on_pause_released(){
	// toggle betewen paused and running modes
	if(rc_get_state()==RUNNING)		rc_set_state(PAUSED);
	else if(rc_get_state()==PAUSED)	rc_set_state(RUNNING);
	return;
}
