#include "motor.h"
#include <roboticscape.h>

#define MOTOR_RIGHT 2           /// channel definition
#define MOTOR_LEFT 1            ///  -||-

double desired_speed = 0.6;           /// speed that we wish to drive on
double duty_right = 0.0;             /// Start the duty-cycle on 0
double duty_left = 0.0;
int encoder_motor_right = 0;        /// just making sure there are no freaky numbers teasing us
int encoder_motor_left = 0;

/* system("stty raw");   */      /// no need for pressing enter after every command (basically only takes in single letters)


void drive_forward(){



}
/// for driving backwards, enter how long you would like to, in seconds.
void drive_backwards(double time){
        rc_set_motor(MOTOR_LEFT,);


}

/// Function for the robot to start driving slowly, so it doesn't just take off at full speed, better for motors
void soft_start(){

    for(i=0;i <= desired_speed;i+=0.1){
        rc_set_motor(MOTOR_RIGHT,i);
        rc_set_motor(MOTOR_LEFT,-i);
        sleep(0.1);
    }

}
/// Function for the robot to stop driving slowly, so it doesn't just stop from full speed, better for the motors
void soft_stop(){

    for(i=desired_speed;i >= 0;i-=0.1){
        rc_set_motor(MOTOR_RIGHT,i);
        rc_set_motor(MOTOR_LEFT,-i);
        sleep(0.1);
    }

}
