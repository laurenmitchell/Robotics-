
#include <Sparki.h>

void moveForward(){
  sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 100);
  sparki.motorRotate(MOTOR_RIGHT,DIR_CW,100);
}
void moveRight(){
  sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 100);
  sparki.motorRotate(MOTOR_RIGHT,DIR_CCW,100);
}

void moveLeft(){
  sparki.motorRotate(MOTOR_LEFT, DIR_CW, 100);
  sparki.motorRotate(MOTOR_RIGHT,DIR_CW,100);
}

void setup()
{
  Serial.begin(9600);
}


void loop() {
  Serial.println("fuck you");
  unsigned long time;
  time = millis();
  sparki.clearLCD();
  sparki.print(time);
  sparki.updateLCD();
  Serial.println(time);
  
  time = millis();
  sparki.moveForward(30);
  
  sparki.clearLCD();
  sparki.print(time);
  sparki.updateLCD();
  moveForward();
 
}
  
