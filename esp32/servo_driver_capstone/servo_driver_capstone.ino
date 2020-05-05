#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int output = 13; //where should servo be controlled from
int pos = 90; //Initial angle

void setup() {
  myservo.attach(output);  // attaches the servo on pin 13 to the servo object
}

void motor_drive(delta) {
  myservo.write(pos+delta);
  pos = pos + delta;
}
