#include <Wire.h>
#include <SparkFun_APDS9960.h>

SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient = 0;
uint16_t red = 0;
uint16_t green = 0;
uint16_t blue = 0;

#define RANGE 10
#define DEFAULT_GREEN 89
#define DEFAULT_BLUE 96
#define DEFAULT_RED 35
#define PIN_JULIETA 12

bool shouldPrint = false;

void setup() {
  Serial.begin(9600);
  apds.init();
  apds.enableLightSensor(false);
  pinMode(PIN_JULIETA, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(2000);
}

void loop(){
  apds.readAmbientLight(ambient);
  apds.readRedLight(red);
  apds.readGreenLight(green);
  apds.readBlueLight(blue);

  red = map(red, 0, 1024, 0, 255);
  green = map(green, 0, 1024, 0, 255);
  blue = map(blue, 0, 1024, 0, 255);

  if(shouldPrint) serialPrint();
  
  if(
  (red >= DEFAULT_RED-RANGE && red <= DEFAULT_RED+RANGE) &&
  (green >= DEFAULT_GREEN-RANGE && green <= DEFAULT_GREEN+RANGE) &&
  (blue >= DEFAULT_BLUE-RANGE && blue <= DEFAULT_BLUE+RANGE)
  ){ 
    if(shouldPrint) Serial.println("verde");
    digitalWrite(PIN_JULIETA,HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
  }
  else{ 
    if(shouldPrint)  Serial.println("preto ou branco");
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(PIN_JULIETA,LOW);
  }
}

void serialPrint(){
  Serial.print(red);
  Serial.print("\t");
  Serial.print(green);
  Serial.print("\t");
  Serial.println(blue);
}
