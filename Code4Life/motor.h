#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED


#define MOTOR_RIGHT 1
#define MOTOR_LEFT 2

float desired_speed; ///For the speed that we wish to drive on
float duty_right;   ///We'll start at the same duty-cycle but we need to be able to change either because of the messages from the encoders
float duty_left;
int encoder_motor_right; /// encoder counter
int encoder_motor_left;

void drive();           /// driving function
void soft_start();      /// start driving slow
void soft_stop();       /// slows down, but doesn't just stop


#endif // MOTOR_H_INCLUDED
