#include <FalconRobot.h>

FalconRobotMotors motors(5, 7, 6, 8);
#define DEFAULT_SPEED 30

#define ID_BLACK 1000
FalconRobotLineSensor leftId(A0);
int leftIdValue;
FalconRobotLineSensor middleId(A1);
int middleIdValue;
FalconRobotLineSensor rightId(A2);
int rightIdValue;

FalconRobotLineSensor left(A3);
int leftValue;
#define LEFT_BORDER 953
FalconRobotLineSensor right(A4);
int rightValue;
#define RIGHT_BORDER 936

FalconRobotLineSensor leftGap(A5);
int leftGapValue;
#define LEFT_GAP_BORDER 987
FalconRobotLineSensor rightGap(A6);
int rightGapValue;
#define RIGHT_GAP_BORDER 964

void setup() {
  Serial.begin(9600);
  delay(1000);
}

void readLineSensors(){
  leftIdValue = leftId.read();
  middleIdValue = middleId.read();
  rightIdValue = rightId.read();
  leftValue = left.read();
  rightValue = right.read();
  leftGapValue = leftGap.read();
  rightGapValue = rightGap.read();
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
  Serial.print(rightValue);
  Serial.print("\t");
  Serial.print(leftGapValue);
  Serial.print("\t");
  Serial.println(rightGapValue);
}

int isInTarget(){
  bool leftIdInTarget = false;
  bool middleIdInTarget = false;
  bool rightIdInTarget = false;
  bool leftInTarget = false;
  bool rightInTarget = false;
  bool leftGapInTarget = false;
  bool rightGapInTarget = false;

  if(leftIdValue >= ID_BLACK-30){ leftIdInTarget = true; }
  if(middleIdValue >= ID_BLACK-30){ middleIdInTarget = true; }
  if(rightIdValue >= ID_BLACK-30){ rightIdInTarget = true; }
  if(leftValue >= LEFT_BORDER-15 && leftValue <= LEFT_BORDER+15){ leftInTarget = true; }
  if(rightValue >= RIGHT_BORDER-15 && rightValue <= RIGHT_BORDER+15){ rightInTarget = true; }
  if(leftGapValue >= LEFT_GAP_BORDER-15 && leftGapValue <= LEFT_GAP_BORDER+15){ leftGapInTarget = true; }
  if(rightGapValue >= RIGHT_GAP_BORDER-15 && rightGapValue <= RIGHT_GAP_BORDER+15){ rightGapInTarget = true; }
  
  if(leftIdInTarget && middleIdInTarget && rightIdInTarget){ return 123; }
  else if(leftIdInTarget && middleIdInTarget){ return 12; }
  else if(middleIdInTarget && rightIdInTarget){ return 23; }
  else if(leftInTarget && rightInTarget){ return 45; }
  else if(!leftIdInTarget && !middleIdInTarget && !rightIdInTarget && !leftInTarget && !rightInTarget){ return 67; }
  else{ return 10; }
}

void lineFollower(bool inGap){
  int leftVelocity, leftDirection = FORWARD;
  int rightVelocity, rightDirection = FORWARD;
  const float Kp = 0.55;
  leftVelocity = (leftValue - LEFT_BORDER)*Kp;
  rightVelocity = (rightValue - RIGHT_BORDER)*Kp;
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
  if(rightVelocity < 30 && leftVelocity < 30){
    motors.drive(DEFAULT_SPEED, FORWARD);
  }else{
    motors.leftDrive(rightVelocity, rightDirection);
    motors.rightDrive(leftVelocity, leftDirection);
  }
}

void loop() {
  readLineSensors();
  printLineSensors();
  switch(isInTarget()){
    case 123:
      //Serial.println("intersecção de 3");
      motors.drive(DEFAULT_SPEED, FORWARD);
      delay(100);
      break;
    case 12:
      //Serial.println("90 esquerda");
      motors.leftDrive(DEFAULT_SPEED, BACKWARD);
      motors.rightDrive(DEFAULT_SPEED, FORWARD);
      delay(420);
      break;
    case 23:
      //Serial.println("90 direita");
      motors.leftDrive(DEFAULT_SPEED, FORWARD);
      motors.rightDrive(DEFAULT_SPEED, BACKWARD);
      delay(420);
      break;
    case 45:
      //Serial.println("condição ideal");
      motors.drive(DEFAULT_SPEED, FORWARD);
      break;
    case 67:
      //Serial.println("gap");
      lineFollower(true);
      break;
    case 10:
      //Serial.println("segue-linha");
      lineFollower(false);
      break;
  }
}
