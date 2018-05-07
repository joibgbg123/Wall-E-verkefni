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

#include <pthread.h>

/// til að spila hljóð....vonandi
#include <sys/types.h>
#include <sys/wait.h>

/// #include <mpg123>

//static volatile double duty_start = 0.4;

#define BASE_DUTY 0.4;
#define SERVO_CH_LEFT 8
#define SERVO_CH_RIGHT 6

// function declarations
void on_pause_pressed();
void on_pause_released();
void Drive();
void servo(int degrees); /// fall til þess að keyra nokkra púlsa á servoa
void play_song(int sound_key);
/*
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void *encoderEntry(void *param)
{
    double duty_start = BASE_DUTY;

/// Motor definitions
    int motor_right = 2;
    int motor_left = 1;

    double dutyLeft = duty_start;
    double dutyRight = -duty_start;

	/// Encoder definitions
    int EncoderLeft = rc_get_encoder_pos(motor_left);
    int EncoderRight = -rc_get_encoder_pos(motor_right);

    //while(EncoderLeft != EncoderRight){
        printf("| Left | Right |\n\r");
        printf("|  %i  |  %i  |\n\r",EncoderLeft,EncoderRight);

        if(EncoderLeft < EncoderRight){
            rc_set_motor(motor_left, dutyLeft + 0.2);
            //rc_set_motor(motor_right, dutyRight - 0.2);
            printf("+0.2 Left");
        }
        else if(EncoderLeft > EncoderRight){
            //rc_set_motor(motor_left, dutyLeft + 0.2);
            rc_set_motor(motor_right, dutyRight - 0.2);
            printf("-0.2 Right");
        }
        else{
            rc_set_motor(motor_left, dutyLeft);
            rc_set_motor(motor_right, dutyRight);
            printf("Equal");
        }

        rc_usleep(500000); /// wait for 0.5 second

    //}


    int *value;
    value = (int*)param;

    int i = 0;
    while(i < value ){
        usleep(1);
        printf("Final Countdown %i\n", i);
        ++i;
    }

    return NULL;
} ///HERE!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

int main(int argc, char* argv[]){
	// always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

	// do your own initialization here
	printf("\nHello BeagleBone\n");
	printf("\nTRY ME\n");
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);

	// done initializing so set state to RUNNING
	rc_set_state(RUNNING);

	// Kveikjum a hardware
	rc_enable_motors();
	rc_enable_servo_power_rail();

	 servo(0);
	 servo(0);

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//pthread_t encoderThread; /// HERE2 !!!!!!!!!

    //int bull = 20;
	//pthread_create(&encoderThread, NULL, encoderEntry, NULL); /// HERE3 !!!!!!!!

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){
		// handle other states
		if(rc_get_state()==RUNNING){
			// do things
			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);
			Drive();
			usleep(1);
		}
		else if(rc_get_state()==PAUSED){
			// do other things
			rc_set_led(GREEN, OFF);
			rc_set_led(RED, ON);
		}
		// always sleep at some point
		usleep(100000);
	}

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//pthread_join(encoderThread, NULL); /// HERE4 !!!

	//int pthread_cancel(pthread_t encoderThread);
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// exit cleanly
	rc_cleanup();
	return 0;
}

///Keyrslan
void Drive()
{

    double duty_start = BASE_DUTY
    double duty1 = duty_start;
    double duty2 = -duty_start;


    /// Motor definitions
    int motor_right = 2;
    int motor_left = 1;


	/// Encoder definitions
	int encoder_right = 2;
	int encoder_left = 1;
	int E1 = 0;
	int E2 = 0;

	/// sound definitions
	// pid_t pid;


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

            rc_set_state(EXITING);
            break;

        case 'b':
            //servo(6,1500); /// void servo(int channel,int pulse) 1500=0°
            //servo(8,1500);
            printf("position 0°\n\r");
            //servo(6,1500); /// void servo(int channel,int pulse) 1500=0°
            //servo(8,1500);
            servo(0);
            break;

        case 'n':
            //servo(6,900); /// 2100=60°
            //servo(8,2100);
            printf("position 60°\n\r");
            //servo(6,900); /// 2100=60°
            //servo(8,2100);
            servo(60);
            break;

        case 'p':
           /*
            int length1 = range(0x70);
            int length2 = range(0x72);
            printf("------skynjarar------ \n \r");
            printf("--------------------- \n \r");
            printf("|skynjari1|skynjari2| \n \r");
            printf("|  %i  |  %i  | \n \r", length1, length2);*/
            rc_i2c_init(1,0x70);
            uint8_t highByte;
            uint8_t lowByte;
            rc_i2c_write_byte(1, 0, 0x51);
            rc_usleep(70000);
            rc_i2c_read_byte(1,2, &highByte);
            rc_i2c_read_byte(1,3, &lowByte);
            int length1 = (highByte << 8) | lowByte;

            rc_i2c_init(1,0x72);
            //uint8_t highByte;
            //uint8_t lowByte;
            rc_i2c_write_byte(1, 0, 0x51);
            rc_usleep(70000);
            rc_i2c_read_byte(1,2, &highByte);
            rc_i2c_read_byte(1,3, &lowByte);
            int length2 = (highByte << 8) | lowByte;

            printf("------skynjarar------ \n \r");
            printf("--------------------- \n \r");
            printf("|skynjari1|skynjari2| \n \r");
            printf("|  %i  |  %i  | \n \r", length1, length2);
            break;

        case 'u':
            play_song(1);
            break;

        /*
        case 'l':
            rc_i2c_init(1,0x70);
            rc_i2c_write_byte(1,0, 0xA0);
            rc_i2c_write_byte(1,0, 0xAA);
            rc_i2c_write_byte(1,0, 0xA5);
            rc_i2c_write_byte(1,0, 0xE4);
            break;
            */



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

void servo(int degrees){
    int i;
    for(i = 0; i < 10;i++){
        rc_send_servo_pulse_us(SERVO_CH_LEFT, 1500+degrees*10); /// pulse 1500 = 0°, pulse 2100 = 60°
        rc_send_servo_pulse_us(SERVO_CH_RIGHT, 1500-degrees*10);
            rc_usleep(1000000/50);
    }
}

void play_song(int song_key){
           pid_t pid;
            pid = fork(); ///Býr til nýjan process


            if(pid < 0) {      ///Ath hvort það hafi tekist
                perror("Fork failed\n");
                exit(1);
            }
            else if(pid == 0){
                ///Hér erum við inn í nýja processnum.
                printf("In child process.\n");  ///Þessari línu má sleppa, aðeins til að villuprófa

                if(1){
                execlp("mpg123", "mpg123", "-q", "./sound/sample.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                return 0;  ///Process hættir keyrslu
                }

            }
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

