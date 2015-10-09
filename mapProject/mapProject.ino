/*******************************************
 Sparki Line-following example
 
 Threshold is the value that helps you 
 determine what's black and white. Sparki's 
 infrared reflectance sensors indicate white 
 as close to 900, and black as around 200.
 This example uses a threshold of 500 for 
 the example, but if you have a narrow line, 
 or perhaps a lighter black, you may need to 
 adjust.
********************************************/

#include <Sparki.h> // include the sparki library
float xi = 0;
float yi = 0;
int xint;
int yint;
float theta = 0;
float r = 2.55; // cm
float d = 8.45; // cm
float motorSpeed = 2.7724; // cm/s
float wheelSpeed = motorSpeed / r; // 1/s
float wheelRight = 0;
float wheelLeft = 0;
unsigned long time;
float deltaTime;
int threshold = 500;
int maxX = 50;
int maxY = 28;

// Mapping
const int x_dim = 16;
const int y_dim = 8;
char my_map[x_dim][y_dim];

void setup() 
{
  sparki.clearLCD();
}

void loop() {
  
  int lineLeft   = sparki.lineLeft();   // measure the left IR sensor
  int lineCenter = sparki.lineCenter(); // measure the center IR sensor
  int lineRight  = sparki.lineRight();  // measure the right IR sensor

  if ( lineLeft < threshold ) // if line is below left line sensor
  {  
    sparki.moveLeft(); // turn left
    wheelLeft = -wheelSpeed;
    wheelRight = wheelSpeed;
  }

  if ( lineRight < threshold ) // if line is below right line sensor
  {  
    sparki.moveRight(); // turn right
    wheelLeft = wheelSpeed;
    wheelRight = -wheelSpeed;
  }

  // if the center line sensor is the only one reading a line
  if ( (lineCenter < threshold) && (lineLeft > threshold) && (lineRight > threshold) )
  {
    sparki.moveForward(); // move forward
    wheelLeft = wheelSpeed;
    wheelRight = wheelSpeed;
  }

  delay(60); // wait 0.1 seconds
  
  deltaTime = float((millis() - time)) / 1000.0;
  xi = xi + cos(theta) * (r * wheelRight / 2 + r * wheelLeft / 2) * deltaTime;
  yi = yi + sin(theta) * (r * wheelRight / 2 + r * wheelLeft / 2) * deltaTime;
  theta = theta + (wheelRight * r / d - wheelLeft * r / d) * deltaTime;
  time = millis();
  
  xint = int(xi / 50 * x_dim) + 1;
  yint = int(-1 * yi / 28 * y_dim);
  if (xint >= x_dim) {
    xint = x_dim - 1;
  }
  if (yint >= y_dim) {
    yint = y_dim - 1;
  }
  if (xint < 0) {
    xint = 0;
  }
  if (yint < 0) {
    yint = 0;
  }
  
  /*sparki.clearLCD();
  sparki.print("\nxi: "); sparki.println(xint);
  sparki.print("\nyi: "); sparki.println(yint);*/
  
  
  my_map[xint][yint] = 1;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      sparki.drawPixel(xint*4+i+16,yint*4+j+8);
    }
  }
  
  /*sparki.clearLCD(); // wipe the screen
  
  sparki.print("Time: "); // show left line sensor on screen
  sparki.println(deltaTime);
  
  sparki.print("Xi: "); // show left line sensor on screen
  sparki.println(xi);
  
  sparki.print("Yi: "); // show center line sensor on screen
  sparki.println(yi);
  
  sparki.print("Theta: "); // show right line sensor on screen
  sparki.println(theta);
  // display all of the information written to the screen*/
  sparki.updateLCD();
}



