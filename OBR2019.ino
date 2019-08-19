#include <FalconRobot.h>

FalconRobotMotors motors(5, 7, 6, 8);
#define DEFAULT_SPEED 25
  
FalconRobotDistanceSensor distanceSensor(2, 3);
#define DEFAULT_DISTANCE 10
int distance;

#define ID_BLACK 990
FalconRobotLineSensor leftId(A0);
int leftIdValue;
FalconRobotLineSensor middleId(A1);
int middleIdValue;
FalconRobotLineSensor rightId(A2);
int rightIdValue;

FalconRobotLineSensor left(A3);
int leftValue;
#define LEFT_WHITE 940
FalconRobotLineSensor right(A4);
int rightValue;
#define RIGHT_WHITE 927

#define PIN_ARDUINO 5

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ARDUINO,INPUT);
  delay(2000);
}

void readLineSensors(){
  leftIdValue = leftId.read();
  middleIdValue = middleId.read();
  rightIdValue = rightId.read();
  leftValue = left.read();
  rightValue = right.read();
}

void printLineSensors(){
  Serial.print(leftIdValue);
  Serial.print("\t");
  Serial.print(middleIdValue);
  Serial.print("\t");
  Serial.print(rightIdValue);
  Serial.print("\t");
  Serial.print(leftValue);
  Serial.print("\t");
  Serial.println(rightValue);
}

void lineFollower(){
  int leftVelocity = 0, leftDirection = FORWARD;
  int rightVelocity = 0, rightDirection = FORWARD;
  const float Kp = 0.55;
  
  leftVelocity = (leftValue - (LEFT_WHITE+30))*Kp;
  rightVelocity = (rightValue - (RIGHT_WHITE+30))*Kp;
  
  if(leftVelocity < 0){
    leftVelocity *= -1;
    leftDirection = BACKWARD;
  }
  if(rightVelocity < 0){
    rightVelocity *= -1;
    rightDirection = BACKWARD;
  }
  
  leftVelocity = map(leftVelocity, 0, 30, 30, 40);
  rightVelocity = map(rightVelocity, 0, 30, 30, 40);
  
  motors.leftDrive(rightVelocity, rightDirection);
  motors.rightDrive(leftVelocity, leftDirection);
}

void rotate(String way){
  if(way == "to left"){
    motors.leftDrive(DEFAULT_SPEED+20, BACKWARD);
    motors.rightDrive(DEFAULT_SPEED, FORWARD);
  }else if(way == "to right"){
    motors.leftDrive(DEFAULT_SPEED, FORWARD);
    motors.rightDrive(DEFAULT_SPEED+20, BACKWARD);
  }
}

void deviateObstacles(){
  distance = distanceSensor.read();
  
  if(distance <= DEFAULT_DISTANCE){
    rotate("to left");
    motors.drive(DEFAULT_SPEED, FORWARD);
    delay(1200);
    rotate("to right");
    motors.drive(DEFAULT_SPEED, FORWARD);
    delay(1200);
    rotate("to right");
    while(middleIdValue < ID_BLACK-10){
      middleIdValue = middleId.read();
      motors.drive(DEFAULT_SPEED, FORWARD);
    }
    motors.stop();
    delay(500);
    rotate("to left");
  }
}

int isInTarget(){
  bool leftIdInTarget = false;
  bool middleIdInTarget = false;
  bool rightIdInTarget = false;
  bool leftInTarget = false;
  bool rightInTarget = false;

  if(leftIdValue >= ID_BLACK-10){ leftIdInTarget = true; }
  if(middleIdValue >= ID_BLACK-10){ middleIdInTarget = true; }
  if(rightIdValue >= ID_BLACK-10){ rightIdInTarget = true; }
  if(leftValue <= LEFT_WHITE+30){ leftInTarget = true; }
  if(rightValue <= RIGHT_WHITE+30){ rightInTarget = true; }

  if(leftIdInTarget && middleIdInTarget && rightIdInTarget){return 123; } // 3
  else if(leftIdInTarget && middleIdInTarget){ return 12; } // 90 e
  else if(middleIdInTarget && rightIdInTarget){ return 23; } // 90 d
  else if(middleIdInTarget && leftInTarget && rightInTarget){ return 45; } // ideal
  else if(!leftIdInTarget && !middleIdInTarget && !rightIdInTarget && leftInTarget && rightInTarget){ return 67; } // gap
  else{ return 10; } //linha
  
  if(leftIdInTarget && middleIdInTarget && rightIdInTarget){return 123; } // 3
  else if(leftIdInTarget && middleIdInTarget){ return 12; } // 90 e
  else if(middleIdInTarget && rightIdInTarget){ return 23; } // 90 d
  else if(middleIdInTarget && leftInTarget && rightInTarget){ return 45; } // ideal
  else if(!leftIdInTarget && !middleIdInTarget && !rightIdInTarget && leftInTarget && rightInTarget){ return 67; } // gap
  else{ return 10; } //linha
}

int solveIntersection(){
  Serial.println(digitalRead(PIN_ARDUINO));
  if(digitalRead(PIN_ARDUINO) == HIGH){ rotate("to left"); }
}

void loop() {
  readLineSensors();
  printLineSensors();
  //deviateObstacles();
  //solveIntersection();
  
  switch(isInTarget()){
    case 123:
      Serial.println("3");
      motors.drive(DEFAULT_SPEED, FORWARD);
      break;
    case 12:
      Serial.println("90 e");
      rotate("to left");
      break;
    case 23:
      Serial.println("90 d");
      rotate("to right");
      break;
    case 45:
      Serial.println("ideal");
      motors.drive(DEFAULT_SPEED, FORWARD);
      break;
    case 67:
      Serial.println("gap");
      motors.drive(DEFAULT_SPEED, FORWARD);
      delay(500);
      break;
    case 10:
      Serial.println("lf");
      lineFollower();
      break;
  }
}
