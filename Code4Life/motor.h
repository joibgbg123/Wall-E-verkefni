#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED


#define MOTOR_RIGHT 2
#define MOTOR_LEFT 1

double desired_speed; ///For the speed that we wish to drive on
double duty_right;   ///We'll start at the same duty-cycle but we need to be able to change either because of the messages from the encoders
double duty_left;
int encoder_motor_right; /// encoder counter
int encoder_motor_left;

void drive_forward();       /// driving forward function
void drive_backwards(double time);     /// driving backwards function, enter for how long you wish to drive backwards in seconds
void soft_start();      /// start driving slow
void soft_stop();       /// slows down, but doesn't just stop


#endif // MOTOR_H_INCLUDED
