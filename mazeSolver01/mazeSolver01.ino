#include <Sparki.h> // include the sparki library

//for sensors
int edgeLeft;
int edgeRight;
int lineCenter; 
int threshold;
boolean foundLWall;
int wallFindType = 0; 
int turnCount = 0; 
//for calibrating
boolean foundLeftWall = false; 
boolean calibrated = false; 
boolean rightWall = false;
boolean foundWall; 
boolean foundDeadEnd; 


void setup() 
{
  //indicates to the user that the program started:
  sparki.beep(440, 300);
  delay(300);
  sparki.beep(880, 500);
  Serial.begin(9600);
}

void loop() {
  threshold = 500;
  readSensors();

  if(calibrated == false)
  {
    calibrateSparki();
  }
  else{

    if(foundDeadEnd && wallFindType == 0){
      sparki.moveRight(90);
      sparki.moveBackward(4);
      sparki.moveForward();
    }
    else if(foundLWall){
      wallFindType = 0;
      sparki.moveForward();
    }
    else if(foundLWall == false && wallFindType == 0){
      sparki.moveStop();
      wallFindType = 1; //now we enter the wall finding routine
      sparki.moveForward(.5);
    }
    if( wallFindType > 0 ){
      wallFind();
    }
  }
  sparki.clearLCD(); // wipe the screen

  sparki.print("Edge Left: "); // show left line sensor on screen
  sparki.println(edgeLeft);

  sparki.print("Edge Right: "); // show left line sensor on screen
  sparki.println(edgeRight);

  sparki.updateLCD(); // display all of the information written to the screen
}
void readSensors()
{
   foundLWall = false;
    foundDeadEnd = false;
    edgeLeft = sparki.edgeLeft();
    edgeRight = sparki.edgeRight();
    lineCenter = sparki.lineCenter();

    if(edgeLeft < threshold){
      foundLWall = true;
    }
    if(edgeLeft < threshold && lineCenter < threshold){
      foundLWall = false;
      foundDeadEnd = true;
    }
  
}

void calibrateSparki()
{
  rightWall = false; //variable so we know if Sparki found the right wall

  if( foundLeftWall == false && calibrated == false ) //if Sparki hasn't found a left wall and hasn't been calibrated yet
  {
    sparki.moveLeft(1); //turn Sparki to the left a tiny bit

    if ( edgeLeft < threshold ) // if there is a line below left edge sensor
    {
      foundLeftWall = true; //Sparki found the left wall
    }
  }

  if( foundLeftWall == true && calibrated == false ) //if Sparki found a wall but isn't calibrated yet
  {
    if( rightWall == false )
    {
      sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 15); //move forward while rotating to the right a little
      sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 55); //move forward while rotating to the right a little
     }

    if( edgeLeft < threshold && edgeRight < threshold ) //if there is a wall beneath both sensors we're at -30 degrees from the wall
    { //this is also called a "known good" position because we know, no matter where Sparki started, where it is now in relation to the wall
      rightWall = true;
      //turn 28 degrees clockwise and scoot back close to the beginning
      sparki.moveForward();
      delay(1000); //this variable got changed a little
      sparki.moveRight(24); //this variable got changed a little
      sparki.moveBackward();
      delay(1000);
      sparki.motorStop(MOTOR_LEFT);
      sparki.motorStop(MOTOR_RIGHT);
      calibrated = true; //set calibrate flag to true so we exit calibrated code
    }
  }
}

 void wallFind(){
    switch(wallFindType){
    case 1: //Sparki might just be off track a little, search around 10 degrees
      while(foundLWall == false && turnCount < 5){ //as long as we haven't found a wall or turned 10 degrees yet
        sparki.moveRight(1);
        turnCount ++;
        readSensors();
      }
      if(turnCount >= 5){
        sparki.moveLeft(5);//turn back to original position
      }
      else{ //otherwise Sparki found a wall
        wallFindType = 0;
        turnCount = 0;
        return;
      }
      turnCount = 0;
      while(foundLWall == false && turnCount < 5){
        sparki.moveLeft(1);
        turnCount ++;
        readSensors();
      }
      if(turnCount >= 5){
        sparki.moveRight(5);//turn back to original position
        wallFindType = 2;//move on to next wall finding type
      }
      else{//otherwise Sparki found a wall
        sparki.println(turnCount);
        wallFindType = 0;
        turnCount = 0;
        return;
      }
      turnCount = 0;
      break;
    case 2://we didn't just lose the wall, there was a turn, check for 90 degree turn
      sparki.moveForward(7);
      sparki.moveLeft(90);
      sparki.moveStop();
      wallFindType = 0;
      break;
    default:
      break;
    }
  }

