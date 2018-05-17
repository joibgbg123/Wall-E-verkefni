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

static volatile int driving_state = 1; ///0 fyrir kyrrstöðu, 1 ef hann er á ferð

/// function declarations
void startup(); // keyrir bara í byrjun
void automation(); //aðal keyrslu fallið sem stjornar skynjurum
void *encodersThread(void *param); //Reglun fyrir encoders
int i2c_1(); //sensor 1
int i2c_2(); // sensor 2
void servo(int degrees); //servo hreyfingar
void play_song(int song_key); //fyrir hljóð
void drive(char direction);

int main()
{
    // always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}

    startup(); //aðal keyrslu fallið


    /// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){

		if(rc_get_state()==RUNNING){
			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);
			automation(); //aðal keyrslu fallið
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

	servo(-10); // byrja með servo a réttum stað, -10 er svo hann sé smá rangeygður

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

        driving_state = 0; //stillir hann sem kyrrstæðan (enginn reglun)

        drive('f');

        int r = rand() % 7; //velur 0-6
        play_song(r);

        drive('s');

        servo(60); /// snúa servo um 60°

        usleep(100000); /// wait for 0.1 second

        //núll stilla fyrir hreyfingar
        ///velur hvaða átt hann beygir í
        int length1 = i2c_1();
        int length2 = i2c_2();

        ///ef það er stutt í hlutinn þa löng beygja
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
        driving_state = 1; //stillum driving state (setja reglun á)

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

            ///regluninn sjálf
            double error = rc_get_encoder_pos(motor_left) + rc_get_encoder_pos(motor_right); //tekur mismun milli encodera
            double adjustment = error/2000.00; //býr til tölugildi til að still af mótorinn
            rc_set_motor(motor_right, dutyRight -= adjustment); //stillir mótorinn

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
    /// 1500 eru 0° og svo margfalda ° með 10 og bæta við fyrir rangsælis og draga frá fyrir réttsælis;
    for(i = 0; i < 10;i++){
        rc_send_servo_pulse_us(SERVO_CH_LEFT, 1500+degrees*10); /// pulse 1500 = 0°, pulse 2100 = 60°
        rc_send_servo_pulse_us(SERVO_CH_RIGHT, 1500-degrees*10); /// mínus því hann lýtur til sitt hvorrar áttarinnar. Hægri skynjari til hægri og öfugt.s
            rc_usleep(1000000/50);
    }
}

void play_song(int song_key)
{
           pid_t pid;
            pid = fork(); ///Býr til nýjan process


            if(pid < 0) {      ///Ath hvort það hafi tekist
                perror("Fork failed\n");
                exit(1);
            }
            else if(pid == 0){
                ///Hér erum við inn í nýja processnum.

                switch(song_key){
                case 0:
                execlp("mpg123", "mpg123", "-q", "./sound/walle3.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;

                case 1:
                execlp("mpg123", "mpg123", "-q", "./sound/runaway.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;

                case 2:
                execlp("mpg123", "mpg123", "-q", "./sound/yoda.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;

                case 3:
                execlp("mpg123", "mpg123", "-q", "./sound/pumba.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;

                case 4:
                execlp("mpg123", "mpg123", "-q", "./sound/chewie.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;

                case 5:
                execlp("mpg123", "mpg123", "-q", "./sound/dalek.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
                break;

                case 6:
                execlp("mpg123", "mpg123", "-q", "./sound/tyrol.mp3", NULL); ///mpg123 látinn spila skránna sem er skrifuð
                // return 0;  ///Process hættir keyrslu
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

    ///Þegar hann tekur af stað
    if(direction == 'w'){
        for(i = 0; i < counter; i++){
            usleep(20000); /// wait for 0.02 second
            L = L - Slow_adjusment;
            R = R + Slow_adjusment;
            rc_set_motor(motor_left, L);
            rc_set_motor(motor_right, R);
        }
    }

    ///Þegar hann bakkar
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

        ///Beygir SMÁ til vinstri
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

    ///Beygir almennilega til hægri
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

        ///Beygir smá til hægri
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

    ///Stoppar yfir höfuð
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



