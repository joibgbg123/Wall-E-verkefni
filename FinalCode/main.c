#include <stdio.h>
#include <stdlib.h>

#include <rc_usefulincludes.h>
#include <roboticscape.h>

#define BASE_DUTY -0.4
#define ADJUSTMENT 0.01
#define Slow_adjusment 0.01

#define motor_right 2
#define motor_left 1

#define SERVO_CH_LEFT 8
#define SERVO_CH_RIGHT 6

static volatile int Thread_switch = 1;
static volatile char encoder_switch = 'w';
static volatile int drivingState = 1; ///1 fyrir take off 0 þegar hann er á ferð

/// function declarations
void Drive();
void selfDrive();
void on_pause_released(); //pause button
void on_pause_pressed(); //pause button 2
void *encoderEntry(void *param); //thread for encoders
pthread_mutex_t lock; //lock for threads and globals
void *sensors(void *param); //thread for sensors
int i2c_1(); //sensor 1
int i2c_2(); // sensor 2
void play_song(int sound_key);  /// a function that plays a song or a sound according to the sound_key

void startSlow(); // start slow and go easy on the dc motors
void servo(int degrees);


int main()
{
	// always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

	printf("\nDestroy all humans\n\r");
	play_song(1);

	///All definitions & functions here below
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);
	rc_set_state(RUNNING);

	/// Activate Hardware
	rc_enable_motors();
	rc_enable_servo_power_rail();

	servo(-10); /// byrja með servo a réttum stað, -10 er svo hann sé smá rangeygður


    /// ENABLE all Threads
/*	pthread_t encoderThread; /// HERE2 !!!!!!!!!
	pthread_create(&encoderThread, NULL, encoderEntry, NULL); /// HERE3 !!!!!!!!*/

	pthread_t sensorThread; /// HERE2 !!!!!!!!!
	pthread_create(&sensorThread, NULL, sensors, NULL);

    /// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){

		if(rc_get_state()==RUNNING){
			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);

			//Drive();
			selfDrive();
		}

		else if(rc_get_state()==PAUSED){

			rc_set_led(GREEN, OFF);
			rc_set_led(RED, ON);

			rc_set_motor(motor_left, 0.0);
            rc_set_motor(motor_right, 0.0);
            printf("Paused mode on \n\r");
		}
		// always sleep at some point
		usleep(100000);
	}

    ///Clean the Thread
	//pthread_join(encoderThread, NULL); /// HERE4 !!!
	//int pthread_cancel(pthread_t encoderThread);

	int pthread_cancel(pthread_t sensorThread);

	// exit cleanly
	servo(0);
	rc_cleanup();
    return 0;
}

void Drive()
{
    /// Motor definitions
    double dutyLeft = BASE_DUTY;
    double dutyRight = -BASE_DUTY;

	/// Encoder definitions
    int EncoderLeft = rc_get_encoder_pos(motor_left);
    int EncoderRight = -rc_get_encoder_pos(motor_right);

    system("stty raw");  /// No need for pressing 'enter' after every input.

    char input = getchar(); //user input
    printf("> ");

    pthread_mutex_lock(&lock);
    switch(input){
        case 'w':
            encoder_switch = 'w';
            printf("| driveF: EncoderLeft | EncoderRight |\n\r");
            printf("| %i | %i |\n\r",EncoderLeft,EncoderRight);

            rc_set_motor(motor_left, dutyLeft + 0.045); ///HARDCODE HERE FOR STRAIGHT
            rc_set_motor(motor_right, dutyRight);
            printf("Autobots! Roll out \n\r");
            break;

        case 's':
            encoder_switch = 'w'; //Thread
            rc_set_motor(motor_left, -dutyLeft - 0.030);
            rc_set_motor(motor_right, -dutyRight);
            printf("Run Away!!! \n\r");
            break;

        case 'a':
            encoder_switch = 'a'; //Thread
            rc_set_motor(motor_left, -dutyLeft/2);
            rc_set_motor(motor_right, dutyRight);
            printf("vinstri beygja \n\r");
            break;

        case 'd':
            encoder_switch = 'a'; //Thread
            rc_set_motor(motor_left, dutyLeft);
            rc_set_motor(motor_right, -dutyRight/2);
            printf("haegri beygja \n\r");
            break;

        case 'f': ///STOPPA
            encoder_switch = 'f'; //Thread
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

void selfDrive()
{
    /// Motor definitions
    double dutyLeft = BASE_DUTY;
    double dutyRight = -BASE_DUTY;

    if(encoder_switch == 'w'){

        int EncoderLeft = rc_get_encoder_pos(motor_left);
        int EncoderRight = -rc_get_encoder_pos(motor_right);

        printf("| Left | Right |\n\r");
        printf("|  %i  |  %i  |\n\r",EncoderLeft,EncoderRight);
        if(drivingState == 1){ //EncoderLeft == 0 && EncoderRight == 0
            startSlow();
            drivingState = 0;
        }

        rc_set_motor(motor_left, dutyLeft + 0.045); ///HARDCODE HERE FOR STRAIGHT
        rc_set_motor(motor_right, dutyRight);
    }

    else if(encoder_switch == 's'){

        //encoder_switch = 'w'; //Thread
        if(drivingState == 1){ //EncoderLeft == 0 && EncoderRight == 0
            startSlow();
            drivingState = 0;
        }
        rc_set_motor(motor_left, -dutyLeft - 0.030); ///HARDCODE HERE FOR STRAIGHT
        rc_set_motor(motor_right, -dutyRight);
    }
    else if(encoder_switch == 'a'){

        //encoder_switch = 'a'; //Thread
        if(drivingState == 1){ //EncoderLeft == 0 && EncoderRight == 0
            startSlow();
            drivingState = 0;
        }
        rc_set_motor(motor_left, -dutyLeft/2);
        rc_set_motor(motor_right, dutyRight);
    }
    else if(encoder_switch == 'd'){

        //encoder_switch = 'a'; //Thread
        if(drivingState == 1){ //EncoderLeft == 0 && EncoderRight == 0
            startSlow();
            drivingState = 0;
        }
        rc_set_motor(motor_left, dutyLeft);
        rc_set_motor(motor_right, -dutyRight/2);
    }

}

void *encoderEntry(void *param)
{
    /// Motor definitions
    //double dutyLeft = BASE_DUTY;
    double dutyRight = -BASE_DUTY;

    while(Thread_switch == 1){

        /// Encoder definitions
        int EncoderLeft = rc_get_encoder_pos(motor_left);
        int EncoderRight = -rc_get_encoder_pos(motor_right);

        if(encoder_switch == 'w' || encoder_switch == 's'){
            /*printf("| Left | Right |\n\r");
            printf("|  %i  |  %i  |\n\r",EncoderLeft,EncoderRight);
            printf("|  %f  |  %f  |\n\r",dutyLeft,dutyRight);*/

            if(EncoderLeft < EncoderRight){
                rc_set_motor(motor_right, dutyRight += ADJUSTMENT);
                printf("+0.01 Left\n\r");
            }
            else if(EncoderLeft > EncoderRight){
                rc_set_motor(motor_right, dutyRight -= ADJUSTMENT);
                printf("-0.01 Right\n\r");
            }
        }

        else if(encoder_switch == 'a' || encoder_switch == 'd'){
            rc_set_encoder_pos(motor_left, 0);
            rc_set_encoder_pos(motor_right, 0);

            printf("| Left | Right |\n\r");
            printf("|  %i  |  %i  |\n\r",EncoderLeft,EncoderRight);
        }

        else if(encoder_switch == 'f'){
            usleep(10000000); /// wait for 10 second
            printf("Im on a break\n\r");
        }

        rc_usleep(800000); /// wait for 0.8 second
    }

    return NULL;
}

void on_pause_released()
{
	// toggle betewen paused and running modes
	if(rc_get_state()==RUNNING)		rc_set_state(PAUSED);
	else if(rc_get_state()==PAUSED)	rc_set_state(RUNNING);
	return;
}

void on_pause_pressed()
{
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

void *sensors(void *param)
{
    while(Thread_switch == 1){

        int length1 = i2c_1();
        int length2 = i2c_2();

   /*     printf("------skynjarar------ \n \r");
        printf("--------------------- \n \r");
        printf("|skynjari1|skynjari2| \n \r");
        printf("|  %i  |  %i  | \n \r", length1, length2); */

        usleep(100000); /// wait for 0.1 second

        pthread_mutex_lock(&lock); ///LOCK IT
        if(length1 <= 40 || length2 <= 40){
            play_song(2);
            drivingState = 1; ///Fyrir take off
            encoder_switch = 's';

            servo(60); /// þegar hann sér eitthvað snýr hann servo um 60°

            usleep(500000); /// wait for 0.5 second

            //encoder_switch = 'a';

            /// if setningin segir bara til um þegar hann er að skoða sig um hvort hann beygji til hægri eða vinstri, 'a' er vinstri, 'd' er hægri.
            /// En hann ber saman lengdirnar sem hann sér í skynjurunum og tekur ákvörðun.
            drivingState = 1; ///Fyrir take off
            if (length1 > length2){

                encoder_switch = 'd';

            }
            else {
                encoder_switch = 'a';
            }
            servo(-1);

            usleep(1000000); /// wait for 1 second

            drivingState = 1; ///Fyrir take off
            encoder_switch = 'w';
        }
        pthread_mutex_unlock(&lock); ///UNLOCK IT

    }

    return NULL;
}

int i2c_1()
{
    rc_i2c_init(1,0x70);
    uint8_t highByte;
    uint8_t lowByte;
    rc_i2c_write_byte(1, 0, 0x51);
    rc_usleep(70000);
    rc_i2c_read_byte(1,2, &highByte);
    rc_i2c_read_byte(1,3, &lowByte);
    int length1 = (highByte << 8) | lowByte;

    return length1;
}

int i2c_2()
{
    rc_i2c_init(1,0x72);
    uint8_t highByte;
    uint8_t lowByte;
    rc_i2c_write_byte(1, 0, 0x51);
    rc_usleep(70000);
    rc_i2c_read_byte(1,2, &highByte);
    rc_i2c_read_byte(1,3, &lowByte);
    int length2 = (highByte << 8) | lowByte;

    return length2;
}

void startSlow()
{
    int i=0;
    int counter = BASE_DUTY/Slow_adjusment;
    double L=0.0;
    double R=0.0;

    if(encoder_switch == 'w'){
        for(i = 0; i < counter; i++){
            usleep(10000); /// wait for 0.01 second
            L = L + Slow_adjusment;
            R = R - Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R);
        }
    }
    else if(encoder_switch == 's'){
        for(i = 0; i < counter; i++){

            usleep(10000); /// wait for 0.01 second
            L = L - Slow_adjusment;
            R = R + Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R);
        }
    }
    else if(encoder_switch == 'a'){
        for(i = 0; i < counter; i++){

            usleep(10000); /// wait for 0.01 second
            L = L - Slow_adjusment;
            R = R - Slow_adjusment;
            rc_set_motor(motor_left, L/2);
            rc_set_motor(motor_right, R);
        }
    }
    else if(encoder_switch == 'd'){
        for(i = 0; i < counter; i++){

            usleep(10000); /// wait for 0.01 second
            L = L + Slow_adjusment;
            R = R + Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R/2);
        }
    }
}

void servo(int degrees){
    int i;
    /// 1500 eru 0° og svo margfalda ° með 10 og bæta við fyrir rangsælis og draga frá fyrir réttsælis;
    for(i = 0; i < 10;i++){
        rc_send_servo_pulse_us(SERVO_CH_LEFT, 1500+degrees*10); /// pulse 1500 = 0°, pulse 2100 = 60°
        rc_send_servo_pulse_us(SERVO_CH_RIGHT, 1500-degrees*10); /// mínus því hann lýtur til sitt hvorrar áttarinnar. Hægri skynjari til hægri og öfugt.s
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

                switch(song_key){
                case 1:
                execlp("mpg123", "mpg123", "-q", "./sound/walle3.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;

                case 2:
                execlp("mpg123", "mpg123", "-q", "./sound/runaway.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;
                }
            }
}


