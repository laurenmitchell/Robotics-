#include <Sparki.h> // include the sparki library
int goal[] = {1,3};
int curr[] = {0,0};
int traveled[16][2] = {};
int adj[4][2] = {};
int i = 0;
int cost[4][4] = {{0, -1, -1, 999},{999, 999, -1, 999},{-1, -1, -1, -1},{999,-1,999,-1} };

void setup() 
{
  Serial.begin(9600);
  sparki.clearLCD();
}

void getAdj() {
  Serial.println("doing getAdj 1");
  if (!((curr[1]-1 < 0) || (cost[curr[0]][curr[1]-1] == 999) || (inTravel(curr[0],curr[1]-1))))
  {
    adj[0][0] = curr[0];
    adj[0][1] = curr[1]-1;
    Serial.println("doing getAdj 1-1");
  }
  if (!((curr[1]+1 < 0) || (cost[curr[0]][curr[1]+1] == 999) || (inTravel(curr[0],curr[1]+1))))
  {
    adj[1][0] = curr[0];
    adj[1][1] = curr[1]+1;
    Serial.println("doing getAdj 1-2");
  }
  if (!((curr[0]-1 < 0) || (cost[curr[0]-1][curr[1]] == 999) || (inTravel(curr[0]-1,curr[1]))))
  {
    adj[2][0] = curr[0]-1;
    adj[2][1] = curr[1];
    Serial.println("doing getAdj 1-3");
  }
  if (!((curr[0]+1 < 0) || (cost[curr[0]+1][curr[1]] == 999) || (inTravel(curr[0]+1,curr[1]))))
  {
    adj[3][0] = curr[0]+1;
    adj[3][1] = curr[1];
    Serial.println("doing getAdj 1-4");
  }
  Serial.println("doing getAdj 2");
}

int dij() {
  Serial.println("doing dijkstra's 1");
  traveled[i][0] == curr[0];
  traveled[i][1] == curr[1];
  for (int a = 0; a<4;a++){
    for (int b = 0; b<2; b++){
      adj[a][b] = -1;
    }
  }
  Serial.println("doing dijkstra's 2");
  getAdj();
  for (int j = 0; j<4;j++){
    if (adj[j][0] != -1){
      if ((cost[curr[0]][curr[1]]+1 < cost[adj[j][0]][adj[j][1]]) || (cost[adj[j][0]][adj[j][1]] == -1)){
        cost[adj[j][0]][adj[j][1]] = cost[curr[0]][curr[1]] + 1;
      }
    }
  }
  lowestNotVisited();
  Serial.println("doing dijkstra's 4");
  if ((curr[0] == goal[0]) && (curr[1] == goal[1])){
    return 0;
  }
  i++;
  Serial.println("doing dijkstra's 6");
  return dij();
}

void lowestNotVisited() {
  Serial.println("doing lowestNotVisited 1");
  int bestcost = 999;
  int x = 0;
  int y = 0;
  for (x = 0;x<4;x++){
    for (y = 0;y<4;y++){
      if (inTravel(x,y) && cost[x][y] < bestcost){
        curr[0] = x;
        curr[1] = y;
        bestcost = cost[x][y];
      }
    }
  }
  Serial.println("doing lowestNotVisited 2");
}

boolean inTravel(int x,int y) {
  for (int j = 0; j <= i;j++){
    if (x == traveled[j][0] && y == traveled[j][1]){
      return true;
    }
  }
  return false;
}

void loop() {
  delay(1000);
  Serial.println("doing dijkstra's");
  dij();
  Serial.println("done with dijkstra's");
//  for (int x = 0;x<4;x++){
//    for (int y = 0;x<4;y++){
//      Serial.print(cost[x][y]);
//    }
//    Serial.print("\t");
//  }
}
