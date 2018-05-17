#include <stdio.h>
#include <stdlib.h>

#include <rc_usefulincludes.h>
#include <roboticscape.h>

#define BASE_DUTY 0.6
#define ADJUSTMENT 0.01
#define Slow_adjusment 0.02

#define motor_right 2
#define motor_left 1

#define SERVO_CH_LEFT 8
#define SERVO_CH_RIGHT 6

static volatile int driving_state = 1; ///0 fyrir kyrrst��u, 1 ef hann er � fer�

/// function declarations
void startup(); // keyrir bara � byrjun
void automation(); //a�al keyrslu falli� sem stjornar skynjurum
void *encodersThread(void *param); //Reglun fyrir encoders
int i2c_1(); //sensor 1
int i2c_2(); // sensor 2
void servo(int degrees); //servo hreyfingar
void play_song(int song_key); //fyrir hlj��
void drive(char direction);

int main()
{
    // always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

    startup(); //a�al keyrslu falli�


    /// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){

		if(rc_get_state()==RUNNING){
			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);
			automation(); //a�al keyrslu falli�
		}

		else if(rc_get_state()==PAUSED){

			rc_set_led(GREEN, OFF);
			rc_set_led(RED, ON);
            printf("Paused mode on \n\r");
		}
		// always sleep at some point
		usleep(100000);
	}

	// exit cleanly
	int pthread_cancel(pthread_t encoderThread);
	servo(0);
	rc_cleanup();
    return 0;
}

void startup()
{
	play_song(0); //play the start function

	///All definitions & functions here below
	//rc_set_pause_pressed_func(&on_pause_pressed);
	//rc_set_pause_released_func(&on_pause_released);
	rc_set_state(RUNNING);

	/// Activate Hardware
	rc_enable_motors();
	rc_enable_servo_power_rail();

	servo(-10); // byrja me� servo a r�ttum sta�, -10 er svo hann s� sm� rangeyg�ur

    drive('w');

	pthread_t encoderThread; /// HERE2 !!!!!!!!!
	pthread_create(&encoderThread, NULL, encodersThread, NULL); /// HERE3 !!!!!!!!
}

void automation()
{
    int length1 = i2c_1();
    int length2 = i2c_2();

    usleep(100000); /// wait for 0.1 second

    if(length1 <= 40 || length2 <= 40){

        driving_state = 0; //stillir hann sem kyrrst��an (enginn reglun)

        drive('f');

        int r = rand() % 7; //velur 0-6
        play_song(r);

        drive('s');

        servo(60); /// sn�a servo um 60�

        usleep(100000); /// wait for 0.1 second

        //n�ll stilla fyrir hreyfingar
        ///velur hva�a �tt hann beygir �
        int length1 = i2c_1();
        int length2 = i2c_2();

        ///ef �a� er stutt � hlutinn �a l�ng beygja
        if(length1 <= 100 && length2 <= 100){
            if (length1 > length2){
                drive('d');
            }
            else {
                drive('a');
            }
        }
        ///annars stutt beygja
        else{
            if (length1 > length2){
                drive('r');
                }
            else {
                drive('l');
            }
        }

        servo(-1);

        usleep(100000); /// wait for 0.1 second

        drive('w');
        driving_state = 1; //stillum driving state (setja reglun �)

    }

}

void *encodersThread(void *param)
{
    /// Motor definitions
    double dutyRight = BASE_DUTY;

    while(rc_get_state()!=EXITING){

        /// Encoder definitions
        int EncoderLeft = -rc_get_encoder_pos(motor_left);
        int EncoderRight = rc_get_encoder_pos(motor_right);

        if(driving_state == 1){
            printf("| Left | Right |\n\r");
            printf("|  %i  |  %i  |\n\r",EncoderLeft,EncoderRight);
            printf("|  %f  |  %f  |\n\r",BASE_DUTY,dutyRight);

            ///regluninn sj�lf
            double error = rc_get_encoder_pos(motor_left) + rc_get_encoder_pos(motor_right); //tekur mismun milli encodera
            double adjustment = error/2000.00; //b�r til t�lugildi til a� still af m�torinn
            rc_set_motor(motor_right, dutyRight -= adjustment); //stillir m�torinn

            /*if(EncoderLeft < EncoderRight){
                rc_set_motor(motor_right, dutyRight -= ADJUSTMENT);
                printf("+%f Left\n\r",ADJUSTMENT);
            }
            else if(EncoderLeft > EncoderRight){
                rc_set_motor(motor_right, dutyRight += ADJUSTMENT);
                printf("-%f Right\n\r",ADJUSTMENT);
            }*/

            rc_set_encoder_pos(motor_left,0);
            rc_set_encoder_pos(motor_right,0);

        }

        else if(driving_state == 0){
            rc_set_encoder_pos(motor_left, 0);
            rc_set_encoder_pos(motor_right, 0);
        }

        rc_usleep(100000); /// wait for 0.1 second
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

void servo(int degrees)
{
    int i;
    /// 1500 eru 0� og svo margfalda � me� 10 og b�ta vi� fyrir rangs�lis og draga fr� fyrir r�tts�lis;
    for(i = 0; i < 10;i++){
        rc_send_servo_pulse_us(SERVO_CH_LEFT, 1500+degrees*10); /// pulse 1500 = 0�, pulse 2100 = 60�
        rc_send_servo_pulse_us(SERVO_CH_RIGHT, 1500-degrees*10); /// m�nus �v� hann l�tur til sitt hvorrar �ttarinnar. H�gri skynjari til h�gri og �fugt.s
            rc_usleep(1000000/50);
    }
}

void play_song(int song_key)
{
           pid_t pid;
            pid = fork(); ///B�r til n�jan process


            if(pid < 0) {      ///Ath hvort �a� hafi tekist
                perror("Fork failed\n");
                exit(1);
            }
            else if(pid == 0){
                ///H�r erum vi� inn � n�ja processnum.

                switch(song_key){
                case 0:
                execlp("mpg123", "mpg123", "-q", "./sound/walle3.mp3", NULL); ///mpg123 l�tinn spila skr�nna sem er skrifu�
                // return 0;  ///Process h�ttir keyrslu
                break;

                case 1:
                execlp("mpg123", "mpg123", "-q", "./sound/runaway.mp3", NULL); ///mpg123 l�tinn spila skr�nna sem er skrifu�
                // return 0;  ///Process h�ttir keyrslu
                break;

                case 2:
                execlp("mpg123", "mpg123", "-q", "./sound/yoda.mp3", NULL); ///mpg123 l�tinn spila skr�nna sem er skrifu�
                // return 0;  ///Process h�ttir keyrslu
                break;

                case 3:
                execlp("mpg123", "mpg123", "-q", "./sound/pumba.mp3", NULL); ///mpg123 l�tinn spila skr�nna sem er skrifu�
                // return 0;  ///Process h�ttir keyrslu
                break;

                case 4:
                execlp("mpg123", "mpg123", "-q", "./sound/chewie.mp3", NULL); ///mpg123 l�tinn spila skr�nna sem er skrifu�
                // return 0;  ///Process h�ttir keyrslu
                break;

                case 5:
                execlp("mpg123", "mpg123", "-q", "./sound/dalek.mp3", NULL); ///mpg123 l�tinn spila skr�nna sem er skrifu�
                // return 0;  ///Process h�ttir keyrslu
                break;

                case 6:
                execlp("mpg123", "mpg123", "-q", "./sound/tyrol.mp3", NULL); ///mpg123 l�tinn spila skr�nna sem er skrifu�
                // return 0;  ///Process h�ttir keyrslu
                break;
                }
            }
}

void drive(char direction)
{
    int counter = BASE_DUTY/Slow_adjusment;
    int i=0;
    double L= 0.00;
    double R= 0.00;

    ///�egar hann tekur af sta�
    if(direction == 'w'){
        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            L = L - Slow_adjusment;
            R = R + Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R);
        }
    }

    ///�egar hann bakkar
    else if(direction == 's'){
        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            L = L + Slow_adjusment;
            R = R - Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R);
        }

        usleep(500000); /// wait for 0.5 second

        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            rc_set_motor(motor_left, L -= Slow_adjusment);
            rc_set_motor(motor_right, R += Slow_adjusment);
        }
    }

    ///Beygir almennilega til vinstri
    else if(direction == 'a'){
        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            L = L + Slow_adjusment;
            R = R + Slow_adjusment;
            rc_set_motor(motor_left, L/2);
            rc_set_motor(motor_right, R);
        }

        usleep(500000); /// wait for 0.5 second

        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            rc_set_motor(motor_left, L -= Slow_adjusment);
            rc_set_motor(motor_right, (R -= Slow_adjusment)/2);
        }
    }

        ///Beygir SM� til vinstri
    else if(direction == 'l'){
        for(i = 0; i < counter/2; i++){
            usleep(20000); /// wait for 0.02 second
            L = L + Slow_adjusment;
            R = R + Slow_adjusment;
            rc_set_motor(motor_left, L/2);
            rc_set_motor(motor_right, R);
        }

        usleep(500000); /// wait for 0.5 second

        for(i = 0; i < counter/2; i++){
            usleep(20000); /// wait for 0.02 second
            rc_set_motor(motor_left, L -= Slow_adjusment);
            rc_set_motor(motor_right, (R -= Slow_adjusment)/2);
        }
    }

    ///Beygir almennilega til h�gri
    else if(direction == 'd'){
        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            L = L - Slow_adjusment;
            R = R - Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R/2);
        }

        usleep(500000); /// wait for 0.5 second

        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            rc_set_motor(motor_left, L += Slow_adjusment);
            rc_set_motor(motor_right, (R += Slow_adjusment)/2);
        }
    }

        ///Beygir sm� til h�gri
    else if(direction == 'r'){
        for(i = 0; i < counter/2; i++){
            usleep(20000); /// wait for 0.02 second
            L = L - Slow_adjusment;
            R = R - Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R/2);
        }

        usleep(500000); /// wait for 0.5 second

        for(i = 0; i < counter/2; i++){
            usleep(20000); /// wait for 0.02 second
            rc_set_motor(motor_left, L += Slow_adjusment);
            rc_set_motor(motor_right, (R += Slow_adjusment)/2);
        }
    }

    ///Stoppar yfir h�fu�
    else if(direction == 'f'){
        L= -BASE_DUTY;
        R= BASE_DUTY;
        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            rc_set_motor(motor_left, L += Slow_adjusment);
            rc_set_motor(motor_right, R -= Slow_adjusment);
        }
    }
}



