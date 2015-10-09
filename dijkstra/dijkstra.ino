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
int startX = 0;
int startY = 0;
int goalX = 1;
int goalY = 3;
int tempcoorx = 0;
int tempcoory = 0;

//this is for smallestdistance function
int mind = 10000;
int xcoors = 0;
int ycoors = 0;


int maze[4][4] = {{0,0,0,1},{1,1,0,1},{0,0,0,0},{1,0,1,0}};
int distances[4][4];
int visited[4][4];

int adjlist[] = {1,1,1,1};

void setup() 
{
  sparki.clearLCD();
}

void loop() {
  
}

void solveMaze() {
  distances[0][0] = 0;
  
  while checkifallvisited(){
    smallestdistance();
    tempcoorx = xcoors;
    tempcoory = ycoors;
  }
}

int checkifallvisited(){
  for(int i = 0; i < 4;i++){
    for(int j = 0; j < 4;j++){
      if visited[i][j] == 1{
        return 0;
      }
      else {
        return 1;
      }
    }
  }
}

int smallestdistance(){
  mind = 10000
  for(int i = 0; i < 4;i++){
    for(int j = 0; j < 4;j++){
      if distances[i][j] <= mind{
        mind = distances[i][j];
        xcoors = i;
        ycoors = j;
      }
    }
  }
  break;
}

void getAdjacent(int curPos[]) 
{
  if(curPos[0] - 1 < 0)
    adjlist[1] = 0;
  if(curPos[0] + 1 > 3)
    adjlist[2] = 0;
  if(curPos[1] - 1 < 0)
    adjlist[0] = 0;
  if(curPos[1] + 1 > 3)
    adjlist[3] = 0;
}  
