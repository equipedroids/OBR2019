/* BIBLIOTECAS */
#include <FalconRobot.h>
#include <Wire.h>
#include <MPU6050.h>

/* CALIBRAGENS */
#define DEFAULT_SPEED 25
#define MAX_SPEED 35
#define DEFAULT_DISTANCE 10
#define DEFAULT_SENSIVITY 3
#define ID_BLACK 960
#define LEFT_WHITE 892
#define RIGHT_WHITE 867

/* MOTORES */
FalconRobotMotors motors(5, 7, 6, 8);

/* ULTRASSÔNICO */
FalconRobotDistanceSensor distanceSensor(2, 3);
int distance;

/* GIROSCÓPIO */
MPU6050 mpu;
float yaw = 0;
float lastYaw = 0;
float timeStep = 0.01;

/* SENSORES DE IDENTIFICAÇÃO */
FalconRobotLineSensor leftId(A0);
int leftIdValue;
FalconRobotLineSensor middleId(A1);
int middleIdValue;
FalconRobotLineSensor rightId(A2);
int rightIdValue;

/* SENSORES DE SEGUE-LINHA */
FalconRobotLineSensor left(A3);
int leftValue;
FalconRobotLineSensor right(A6);
int rightValue;

/* ARDUINOS EXTERNOS */
#define LEFT_COLOR_NANO 8
#define RIGHT_COLOR_NANO 9

/* TEMPO */
unsigned long int actualTime = 0;
unsigned long int distanceLastTime = 0;
unsigned long int motorStallLastTime = 0;

/* FUNÇÃO INICIAL */
void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);
  mpu.calibrateGyro();
  mpu.setThreshold(DEFAULT_SENSIVITY);
  delay(2000);
}

/* FUNÇÃO PRINCIPAL */
void loop() {
  readLineSensors();
  deviateObstacles();
  /*
  switch(isInTarget()){
    case 123:
      switch(solveIntersection()){
        case 12:
          moveTo("turn180");
          break;
        case 1:
          moveTo("left");
          delay(500);
          break;
        case 2:
          moveTo("right");
          delay(500);
          break;
        case 0:
          moveTo("forward");
          break;
      }
      moveTo("forward");
      break;
    case 12:
      moveTo("left");
      break;
    case 23:
      moveTo("right");
      break;
    case 45:
      moveTo("forward");
      break;
    case 10:
      lineFollower();
      break;
    case 0:
      break;
  }
  */
}
/* FUNÇÃO PARA REALIZAR MOVIMENTOS  */
void moveTo(String way){
  if(way == "left"){
    motors.leftDrive(DEFAULT_SPEED, BACKWARD);
    motors.rightDrive(DEFAULT_SPEED, FORWARD);
  }else if(way == "right"){
    motors.leftDrive(DEFAULT_SPEED, FORWARD);
    motors.rightDrive(DEFAULT_SPEED, BACKWARD);
  }else if(way == "forward"){
    motors.leftDrive(DEFAULT_SPEED, FORWARD);
    motors.rightDrive(DEFAULT_SPEED, FORWARD);
  }else if(way == "backward"){
    motors.leftDrive(DEFAULT_SPEED, BACKWARD);
    motors.rightDrive(DEFAULT_SPEED, BACKWARD);
  }else if(way == "turn180"){
    motors.leftDrive(DEFAULT_SPEED*2, FORWARD);
    motors.rightDrive(DEFAULT_SPEED*2, BACKWARD);
    delay(500);
  }
}

/* FUNÇÃO DE LEITURA DOS SENSORES DE REFLECTÂNCIA */
void readLineSensors(){
  leftIdValue = leftId.read();
  middleIdValue = middleId.read();
  rightIdValue = rightId.read();
  leftValue = left.read();
  rightValue = right.read();
}

/* FUNÇÃO PARA CALIBRAGEM MANUAL DOS SENSORES DE REFLECTÂNCIA */
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

/* FUNÇÃO PARA LEITURA DO SENSOR GIROSCÓPIO */
void readGyroSensor(){
  Vector norm = mpu.readNormalizeGyro();
  yaw = yaw + norm.ZAxis * timeStep;
}

/* FUNÇÃO PARA EXECUTAR GIROS */
void turnGyro(int deg, String way, bool higher){
  if(higher){
    while(yaw < deg){
      readGyroSensor();
      moveTo(way);
    }
  }else if(!higher){
    while(yaw > deg){
      readGyroSensor();
      moveTo(way);
    }
  }
  lastYaw = yaw;
  yaw = 0;
}

/* FUNÇÃO PARA IDENTIFICAR E DESVIAR DE OBSTÁCULOS */
void deviateObstacles(){
  distance = distanceSensor.read();
  readGyroSensor();
  turnGyro(60, "left", true);
  moveTo("forward"); delay(1000);
  turnGyro(-lastYaw, "right", false);
  moveTo("forward"); delay(1000);
  turnGyro(lastYaw, "right", false);
  readLineSensors();
  while(middleIdValue <= ID_BLACK-10){
    readLineSensors();
    moveTo("forward");
  }
  turnGyro(-lastYaw, "left", true);
  motors.stop();
  delay(5000);
}

/* FUNÇÃO PARA IDENTIFICAR O POSICIONAMENTO DO ROBÔ */
int isInTarget(){
  bool leftIdInTarget = false;
  bool middleIdInTarget = false;
  bool rightIdInTarget = false;
  bool leftInTarget = false;
  bool rightInTarget = false;

  if(leftIdValue >= ID_BLACK-10){ leftIdInTarget = true; }
  if(middleIdValue >= ID_BLACK-10){ middleIdInTarget = true; }
  if(rightIdValue >= ID_BLACK-10){ rightIdInTarget = true; }
  if(leftValue >= LEFT_WHITE){ leftInTarget = true; }
  if(rightValue >= RIGHT_WHITE){ rightInTarget = true; }
  
  if(leftIdInTarget && middleIdInTarget && rightIdInTarget){ return 123; } // 3
  else if(leftIdInTarget && middleIdInTarget){ return 12; } // 90 e
  else if(middleIdInTarget && rightIdInTarget){ return 23; } // 90 d
  else if(middleIdInTarget && !leftInTarget && !rightInTarget){ return 45; } // ideal
  else if(leftInTarget || rightInTarget){ return 10; } //linha
  else{ return 0; } //excessões
}

/* FUNÇÃO PARA IDENTIFICAR FITAS VERDES */
int solveIntersection(){
  if(digitalRead(LEFT_COLOR_NANO) == HIGH && digitalRead(RIGHT_COLOR_NANO) == HIGH){ return 12; }
  else if(digitalRead(LEFT_COLOR_NANO) == HIGH){ return 1; }
  else if(digitalRead(RIGHT_COLOR_NANO) == HIGH){ return 2; }
  else{ return 0; }
}

/* FUNÇÃO PARA EXECUTAR O SEGUE-LINHA */
void lineFollower(){
  int leftVelocity = 0, leftDirection = FORWARD;
  int rightVelocity = 0, rightDirection = FORWARD;
  const float Kp = 0.5;
  
  leftVelocity = (leftValue - LEFT_WHITE)*Kp;
  rightVelocity = (rightValue - RIGHT_WHITE)*Kp;
  
  if(leftVelocity < 0){
    leftVelocity *= -1;
    leftDirection = BACKWARD;
  }
  if(rightVelocity < 0){
    rightVelocity *= -1;
    rightDirection = BACKWARD;
  }
  
  leftVelocity = map(leftVelocity, 0, DEFAULT_SPEED, DEFAULT_SPEED, MAX_SPEED);
  rightVelocity = map(rightVelocity, 0, DEFAULT_SPEED, DEFAULT_SPEED, MAX_SPEED);
  
  motors.leftDrive(rightVelocity, rightDirection);
  motors.rightDrive(leftVelocity, leftDirection);
}
