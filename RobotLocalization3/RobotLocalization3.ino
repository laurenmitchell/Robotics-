/*******************************************
 Sparki Robot Localization example

 Description:
 This Code contains a simple Kinematics example of translating a Robots position into that of a World frame. It also demonstates the errors and their variations 
 when performing Loop Closure on a Kinematic System with different configuration parameters.
 
 Procedure for Running this code:
 1) Once sparki is On with the uploaded code and placed on the Line Following Course, press 5 on the Sparki Remote to begin Caliberation
 2) Sparki will turn to the right and return back to its original Position
 3) Then press 5 again to start the Loop Closure Algorithm
 4) The Output shown on Sparki's Screen is in the following format:
   Line 1: Sensor Array: 00100      // Shows the boolean representation for each sensors' value (0(No Black Line under sensor) / 1(Black Line under sensor))
   Line 2: 0.00, 0.00               // Error (deviation from the line), Correction (corrective value applied to the motors to get sparki back on the line)
   Line 3: 80.00, 80.00             // Speed of Left Motor (-100 to 100), Speed of Right Motor (-100 to 10)
   Line 4: 2.60, 0.00, 0.00         // Vxi, Vyi, VTheta of  World Frame {I}
   Line 5: 0.00, 0.00, 0.00         // Change in Vxi, Vyi, VTheta of World Frame {I} with respect to previous iteration 
   Line 6: 7.43, 0.00, 0.00         // Xi, Yi, Theta (Pose of sparki) in World Frame {I}
   Line 7: 0.13, 0.00, 0.00         // Change in Xi, Yi, Theta of World Frame {I} with respect to previous iteration
 5) If sparki detects Loop Closure, it will stop.
 6) To continue then, press 5 on the sparki remote
 7) To stop sparki anytime during while its functioning, press Stop(Square) Button on the sparki remote
 8) To tell sparki to continue his functioning, press 5 on the sparki remote
 
 ********************************************/

#include <Sparki.h> // include the sparki library

//==============================DEFINE PARAMETERS================================
// Radius of Sparki Wheel
#define r  (WHEEL_DIAMETER_CM/2.0)

// Axle Distance i.e. Distance between the two wheels
#define d  TRACK_WIDTH_CM
/* r and d can be provided in any metric units you are comfortable with. I have
specified these parameters in cm. */

// Max speed parameter
#define max_speed 100
/* This is the maximum speed as a percentage which can be given to Sparki Motor Functions */

// Delay gap every iteration in ms
#define delay_gap 0
/* How much delay needs to be inserted within the loop(). Theoretically, best performance should
be obtained when the loop() runs continuously and delay is 0 */

// Define IR Reflectance Sensor Threshold
#define IR_Reflectance_threshold 700

// Base Speed, Range:20 to 80
#define BaseSpeed 80
/* Base speed  is the speed of both the motors in percent when there is no correction applied to them*/

//===============================================================================

//================Sensor Weights=================
/* These weights are multiplicative and are used to generate error (deviation from the line) */
#define wEdgeLeft     -0
#define wLineLeft     -35
#define wLineCenter   0
#define wLineRight    35
#define wEdgeRight    0
//===============================================

//=========Loop Closure Variables================
// min Time after which to check Loop Closure in ms
#define checkLoopClosureAfter 30000
/* Tells the algorithm after how long to check for loop closure once the Bot has started moving */

// max xi Error
#define max_xi  0.2
/* This is the allowable xi error which can be overlooked during detection of Loop Closure */

// max yi Error
#define max_yi  5
/* This is the allowable yi error which can be overlooked during detection of Loop Closure */

// max thetai Error in Radians
#define max_theta  30 * DEG_TO_RAD
/* This is the allowable theta error which can be overlooked during detection of Loop Closure */

#define inRange(var,max) ((var)<(max) && (var)>-(max))
/* Checks: -max < var < max */
//===============================================

/* Description of Variables
vxi - Current Value of Vxi (Velocity in x direction of World Frame {I})
vyi - Current Value of Vyi (Velocity in y direction of World Frame {I})
vtheta - Current Value of VTheta (Angular Velocity around z direction of World Frame {I})
xi - Current Value of Xi (Displacement in x direction of World Frame {I})
yi - Current Value of Yi (Displacement in y direction of World Frame {I})
theta - Current Value of Theta (Angular Displacement around z direction of World Frame {I})
*/
double vxi=0,vyi=0,vtheta=0, xi=0, yi=0, theta=0;

/*  Description of Variables
conv - Constant multiplier that converts (0-100) Percent Velocity to Angular Velocity in Rad/s (deduced by caliberate())
time - Keeps mark of the start time of every iteration
initial_time - Keeps mark of the start time of the entire Algorithm (Loop Closure)
*/
double conv = 0;
double time=0;
double initial_time=0;

// velocity screws (scaled to range from +100 to -100)
float vL=BaseSpeed,vR=BaseSpeed;

/* Desciption of Variables 
enum sensor - Easy notation of all 5 sensors
sensors[5] - boolean array which keeps track of the sensor values discretized to True/False
*/
enum sensor{EdgeLeft,LineLeft,LineCenter,LineRight,EdgeRight};
bool sensors[5]={false,false,false,false,false};

// Stores error of Robot from line (i.e. Deviation from the line)
double error=0;


// Mapping
const int x_dim = 16;
const int y_dim = 8;
char my_map[x_dim][y_dim];

// Deduces constant conv which converts Velocity Percent Speed applied to motor to Angular Speed in Rad/s
inline void caliberate()
{
  sparki.speed(max_speed);  // set speed of sparki to Max
  int omega=ceil(((2*M_PI*r)/(float)d)*RAD_TO_DEG);  // Turn measure for one full rotation of wheel
  double mils=millis();  // records number of miliseconds passed after robot boot
  sparki.moveRight(omega);
  double mils2=millis();  // This will help to get the time required by Sparki in ms to rotate its wheels one full rotation at its fastest possible
  sparki.moveLeft(omega);
  conv=M_PI/(100.0*((mils2-mils)/1000.0));  // generic formula = pi/(max_speed * t)
}

// Read IR Reflectance Sensor Values and discretize them to True/False
inline void ReadSensors()
{
  sensors[EdgeLeft]=(bool)(sparki.edgeLeft() < IR_Reflectance_threshold);
  sensors[LineLeft]=(bool)(sparki.lineLeft() < IR_Reflectance_threshold);
  sensors[LineCenter]=(bool)(sparki.lineCenter() < IR_Reflectance_threshold);
  sensors[LineRight]=(bool)(sparki.lineRight() < IR_Reflectance_threshold);
  sensors[EdgeRight]=(bool)(sparki.edgeRight() < IR_Reflectance_threshold);  
}

// Find error (i.e. deviation of sparki's position from the Line)
inline float getError()
{
  // If no line detected, keep previous error as current error. Eventually sparki will return back to the line!
  if(!sensors[EdgeLeft] && !sensors[LineLeft] && !sensors[LineCenter] && !sensors[LineRight] && !sensors[EdgeRight])
    return error;
  
  // Calculate error as the product of the Weight of each sensor and the discretized (1/0) value of sensor   
  error= wEdgeLeft*(float)sensors[EdgeLeft]+
         wLineLeft*(float)sensors[LineLeft]+
         wLineCenter*(float)sensors[LineCenter]+
         wLineRight*(float)sensors[LineRight]+
         wEdgeRight*(float)sensors[EdgeRight];
         
  // if error is 0, keep both motor Velocities same and equal to the default velocity       
  if(!error)
    vL=vR=BaseSpeed;

  return error;
}

// Blocking Function which displays a message on the screen and returns from its scope only after a 5 Key is pressed from the sparki remote
void press5(char *message)
{
  sparki.println(message);
  sparki.updateLCD();
  while(sparki.readIR()!=28)
    delay(1000);
}

// Setup to initialize Sparki. This is done only once. Includes Caliberation of Sparki
void setup() 
{
  sparki.clearLCD();
  press5("Press 5 on remote to caliberate...");
  caliberate();
  press5("Press 5 on remote \nagain to begin...");
  sparki.clearLCD();
  initial_time=time=millis();
}

// Given a Velocity V, ranging from -100 to 100 and Motor Type (MOTOR_LEFT or MOTOR_RIGHT), determine whether the motor has to rotate CounterClockwise (DIR_CCW) or Clockwise (DIR_CW)
inline int determineDirection(int V,int motor)
{
  switch(motor)
  {
    case MOTOR_LEFT:
                {
                  if(V>=0)
                    return DIR_CCW;
                  else
                    return DIR_CW;     
                }
                
    case MOTOR_RIGHT:
                {
                  if(V>=0)
                    return DIR_CW;
                  else
                    return DIR_CCW;
                }
  }
  return 0;
}

// Prevent velocities from exceeding legal values of -100 to 100
inline int boundVelocities(float V)
{
  if(V>max_speed)
    return max_speed;
  else if(V<-max_speed)
    return -max_speed;
  else
    return (int)V;
}

// High Level Motor Function which calls sparki MotorRotate with appropriate parameters
inline void moveBot(int vL, int vR)
{
  sparki.motorRotate(MOTOR_RIGHT,determineDirection(vR,MOTOR_RIGHT),abs(vR));
  sparki.motorRotate(MOTOR_LEFT,determineDirection(vL,MOTOR_LEFT),abs(vL));
}

// Limit the Range of Theta from +/- 0 to +/- 2.pi
inline double limitTheta(double theta)
{
  if(theta>2*M_PI)
    theta-=2*M_PI;
  else if(theta<-2*M_PI)
    theta+=2*M_PI;
  return theta;
}

// Loop Function which contains Loop Closure Algorithm and Line Following Algorithm
void loop() {
  
  ReadSensors();
  
  // Correction to the two Motors will be half of negative of error
  double Correction=(-getError())/2.0;

  // New Velocity Screws (Line Following Algorithm)
  vL-=Correction;
  vR+=Correction;
  
  // Prevent Velocities from going beyond legal values of -100 to 100
  vR=boundVelocities(vR);
  vL=boundVelocities(vL);
  
  // Give the velocity Screws to the respective motors
  moveBot(vL,vR);
  
  delay(delay_gap);    // Delay for the specified time

  //sparki.clearLCD(); // wipe the screen
  
  //sparki.print("Sensor Array: ");  // Print the Boolean representation of the 5 Sensors
  //for(int i=EdgeLeft;i<=EdgeRight;i++)
    //sparki.print((int)sensors[i]);
  
  //sparki.println("");
  
  /* Description of Variables
  prevVxi - save Vxi before calculating updated Vxi
  prevVyi - save Vyi before calculating updated Vyi
  prevVtheta - save VTheta before calculating updated VTheta
  */
  double prevVxi=vxi,prevVyi=vyi,prevVtheta=vtheta;
  
  /* Description of Variables
  phiL - Stores the angular velocity of Left Motor on the Robot Frame {R}
  phiR - Stores the angular velocity of Right Motor on the Robot Frame {R}
  stem - intermediate variable used from calculating new Vxi and Vyi
  */
  double phiL=conv*vL, phiR=conv*vR;
  double stem=(r/2.0)*(phiL+phiR);
  vxi=cos(theta)*stem;
  vyi=sin(theta)*stem;
  vtheta=(r/(float)d)*(phiR-phiL);
    
  // save time before updating it so as to enable finding time elapsed between consecutive iterations
  double prevTime=time;
  time=millis();
  double duration=(time-prevTime)/1000.0;      // convert to sec from ms
  
  /* Description of Variables
  changeXi - compute the instantaneous displacement over Xi by multiplying Vxi with duration of iteration 
  changeYi - compute the instantaneous displacement over Yi by multiplying Vyi with duration of iteration
  changeTheta - compute the instantaneous angular displacement over Theta by multiplying VTheta with duration of iteration
  */
  double changeXi=(vxi*duration),
        changeYi=(vyi*duration),
        changeTheta=(vtheta*duration);

  // Update Spaki Pose variables xi, yi and theta
  xi+=changeXi;
  yi+=changeYi;
  theta+=changeTheta;
  theta=limitTheta(theta);

  // Print Updated Sparki Pose in World Frame {I}
  //sparki.print("\nxi: "); sparki.println(xi);
  //sparki.print("\nyi: "); sparki.println(yi);
  //sparki.print("\ntheta: "); sparki.println(theta*RAD_TO_DEG);
  
  int xint = int(xi / 50 * x_dim) + 1;
  int yint = int(-1 * yi / 28 * y_dim);
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
  //sparki.drawPixel(xint+32,yint+16);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      sparki.drawPixel(xint*4+i+32,yint*4+j+8);
    }
  }
  
  sparki.updateLCD(); // display all of the information written to the screen
  
  // Check for Loop Closure after specified amount of time. Check whether xi,yi and theta are at (0,0,0) +/- some allowable error
  if(millis()-initial_time>checkLoopClosureAfter && 
        inRange(xi,max_xi) && inRange(yi,max_yi) && inRange(theta,max_theta))
  {
    sparki.moveStop();
    press5("Loop Closed. Press 5 to continue...");
    sparki.clearLCD();
    initial_time=time=millis();      // record time before starting in order to record elapsed time between consecutive iterations
  }
  
  // Stop Sparki if the Stop Button (Square) has been pressed on the remote
  if(sparki.readIR()==64)
  {
    sparki.moveStop();
    
    delay(1000);
    
    // Start Sparki if 5 is pressed on the sparki remote
    while(sparki.readIR()!=28)
      delay(1000);
    
    time=millis();      // record time before starting in order to record elapsed time between consecutive iterations
  }
}
