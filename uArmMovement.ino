
#include "uArm.h"
#include "uArmAPI.h"
#define USE_SERIAL_CMD  1 // 1: use serial for control  0: just use arduino to control(release ROM and RAM space)

unsigned long tickStartTime = millis(); // get timestamp;
static void Init();
int box1[] = {-151.5, 130, 155};
int box2[] = {-154, 234, 156};
int input = 0;
//int storage[] = {};

void setup()
{
  Serial.begin(9600);
  uArmInit(); // Don't remove
  //uarm.init();
  debugPrint("debug start"); // uncomment DEBUG in uArmConfig.h to use debug function
  //pinMode(2,INPUT); //TipSensor
  // TODO
  moveTo(0, 150, 150); //Neutral Position coordinates
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
//  moveTo(box1[0],box1[1],box1[2]);  //Move to Box 1
//  Serial.println("@1"); // report ready
//  reportPos(); //Serial Prints the current position
//  int boxZ = box1[2];
//  while (getTip()) {
//    boxZ = boxZ - 1;
//    //box1[1] = box1[1] + 5;
//    //delay(1000);
//    moveTo(box1[0],box1[1],boxZ, 50);  //Move to Box 1
//    Serial.println("@1"); // report ready
//    reportPos(); //Serial Prints the current position
//    //delay(150);
//  }
//  delay(1000);
//  pumpOn();
//  reportPos();
//  delay(1000);
//  moveTo(box1[0],box1[1], 200, 100);
//  delay(3000);
//  moveTo(207.5, 128.6, 150, 100);
//  delay(3000);
//  pumpOff();
//  delay(3000);
//  moveTo(0, 150, 150); //Neutral Position coordinates
//  Serial.println("@1"); // report ready
//  reportPos(); //Serial Prints the current position
  

}

void loop()
{
  run(); // Don't remove
  Serial.println("Please input 1 or 2");
  while (Serial.available() == 0) {} //do nothing until input
  if (Serial.available() > 0){
    input = Serial.read();
    //input.toCharArray
    Serial.println(input - '0');
    if (input == '1'){
      getFood(box1);
      //input = '0';
    }else if (input == '2'){
      getFood(box2);
      //input = '0';
    }/*else {
      Serial.println("Please input 1 or 2");
    }*/
    //Serial.print("Input : "); 
    //Serial.println(atoi(input));
    
  }
  // TODO
  
  /*moveTo(box1[0],box1[1],box1[2]);  //Move to Box 1
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  delay(1000);
  moveTo(box2[0],box2[1],box2[2]);  //Move to Box 2
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  delay(1000);
  */
}

void getFood(int storage[])
{
  //delay(1000);
  int storageZ = storage[2];
  moveTo(storage[0],storage[1],storageZ);  //Move to Box 1
  delay(1000);
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  while (getTip()) {
    storageZ = storageZ - 1;
    moveTo(storage[0],storage[1],storageZ, 25);  //Move to Box 1
    //Serial.println("@1"); // report ready
    //reportPos(); //Serial Prints the current position
    delay(100);
  }
  delay(1000);
  pumpOn();
  reportPos();
  delay(1000);
  moveTo(storage[0],storage[1], 200, 100);
  delay(1000);
  moveTo(207.5, 128.6, 200, 100); //To the cutting Module
  delay(1000);
  pumpOff();
  delay(1000);
  moveTo(0, 150, 150); //Neutral Position coordinates
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  

}
// time out every TICK_INTERVAL(50 ms default)
void tickTimeOut()
{
  
}

////////////////////////////////////////////////////////////
// DO NOT EDIT
static void Init()
{
  uArmInit(); // Don't remove
  service.init();

  #if USE_SERIAL_CMD == 1
  serialCmdInit();
  

  #endif
}

void run()
{
  #if USE_SERIAL_CMD == 1
  handleSerialCmd();
  #endif

  manage_inactivity(); // Don't remove
}

void tickTaskRun()
{
  tickTimeOut();

    buttonPlay.tick();
    buttonMenu.tick();
#ifdef MKII
    ledRed.tick();
    service.btDetect();
#endif    
}

void manage_inactivity(void)
{
#if USE_SERIAL_CMD == 1
  getSerialCmd(); // for serial communication
#endif
  service.run();  // for led, button, bt etc.

  // because there is no other hardware timer available in UNO, so use a soft timer
  // it's necessary for button,led, bt
  // so Don't remove it if you need them
  if(millis() - tickStartTime >= TICK_INTERVAL)
  {
    tickStartTime = millis();
    tickTaskRun();
  }   
}
