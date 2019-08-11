#include <FalconRobot.h>

FalconRobotMotors motors(5, 7, 6, 8);
#define DEFAULT_SPEED 30

FalconRobotLineSensor left(A3);
int leftValue;
#define LEFT_WHITE 880
#define LEFT_BLACK 975
#define LEFT_BORDER 960
FalconRobotLineSensor right(A4);
int rightValue;
#define RIGHT_WHITE 860
#define RIGHT_BLACK 965
#define RIGHT_BORDER 910

void setup() {
  Serial.begin(9600);
  delay(1000);
}

void readLineSensors(){
  leftValue = left.read();
  rightValue = right.read();
}

void printLineSensors(){
  Serial.print("left: ");
  Serial.print(leftValue);
  Serial.print("\t");
  Serial.print("right: ");
  Serial.println(rightValue); 
}

int isInTarget(){
  bool leftInTarget = false;
  bool rightInTarget = false;
  if(leftValue >= LEFT_BORDER-15 && leftValue <= LEFT_BORDER+15){ leftInTarget = true; }
  if(rightValue >= RIGHT_BORDER-15 && rightValue <= RIGHT_BORDER+15){ rightInTarget = true; }
  if(leftInTarget && rightInTarget){ return 1; }
  else{ return 2; }
}

void lineFollower(){
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
  switch(isInTarget()){
    case 1:
      motors.drive(DEFAULT_SPEED, FORWARD);
      break;
    case 2:
      lineFollower();
      break;
  }
}
