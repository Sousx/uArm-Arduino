
/*Code for Uarm Arduino Uno movement and receiving input from Mega
 * 
 * 
 */

#include "uArm.h"
#include "uArmAPI.h"

unsigned long tickStartTime = millis(); // get timestamp;
static void Init();

int box1[] = {77.1, 127.2, 152.87};
int box2[] = {-17.8, 134.8, 152.87};
int diceBox[] = {119.8 , 282, 250}; // directly left of the storage area
//int sliceBox[] = {50, 182,155};  // slightly more left than above coordinates
//int input = 0;
int box1down[] = {48.6,101,75}; //coordinates of center of box 1
int box2down[] = {-17.8,105.2,74.06};

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
//char cutFromMega[numChars] = {0};
int boxFromMega = 0;
//float floatFromPC = 0.0;

boolean newData = false;

void setup()
{

  Serial.begin(9600);
  Init(); // Don't remove
  //uarm.init();
  //debugPrint("debug start"); // uncomment DEBUG in uArmConfig.h to use debug function
  //pinMode(2,INPUT); //TipSensor
  // TODO
  moveTo(0, 150, 150, 50); //Neutral Position coordinates
  Serial.println("@1"); // report ready
  //reportPos(); //Serial Prints the current position
  Serial.println("Please input <1> or <2>");
}

void loop(){
  //int processLoc[] = {0,0,0};
  run(); // Don't remove
  recvWithStartEndMarkers();
 
   
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();
    //showParsedData();
    
    //Serial.print("Choose box ... ");
    //Serial.println(receivedChars);
    //Serial.println(receivedChars[1] - '0');
//    if(strcmp(cutFromMega,"d")== 0){
//     // processLoc = diceBox;
//      memcpy(processLoc, diceBox,sizeof(diceBox));
//    } else if (strcmp(cutFromMega,"s")==0){
//     // processLoc = sliceBox;
//      memcpy(processLoc, sliceBox,sizeof(sliceBox));
//    } else {
//      Serial.print("Inoperable process selected you chose: ");
//      Serial.println(cutFromMega);
//      Serial.write('f');
//    }
    if (boxFromMega == 1){
      Serial.println("Get food from box 1");
      //Serial.println(cutFromMega);
      getFood(box1);//, processLoc);
    }else if (boxFromMega == 2){
      Serial.println("Get food from box 2");
      //Serial.println(cutFromMega);
      getFood(box2);//, processLoc);
    } else {
      Serial.println("You didn't pick a food box, jackass.");
    }
    //Serial.write(cutFromMega);
    Serial.println("Please input <1> or <2>");
    newData = false;
  }
  
  //Serial.print("Input : "); 
  //Serial.println(atoi(input));    
}


void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() == 0) {} //do nothing until input
  while ((Serial.available() > 0) && newData == false){
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

//    strtokIndx = strtok(tempChars,",");      // get the first part - the string
//    strcpy(cutFromMega, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(tempChars, ","); // this continues where the previous call left off
    boxFromMega = atoi(strtokIndx);     // convert this part to an integer

    //strtokIndx = strtok(NULL, ",");
    //floatFromPC = atof(strtokIndx);     // convert this part to a float

}

//============

void showParsedData() {
//    Serial.print("Type of cut: ");
//    Serial.println(cutFromMega);
    Serial.print("Food from box: ");
    Serial.println(boxFromMega);
    //Serial.print("Float ");
    //Serial.println(floatFromPC);
}

void getFood(int storage[])//, int processing[])
{
  //delay(1000);
  int storageZ = storage[2];
  int storageY = storage[1];
  int storageX = storage[0];
  int everyZ = 0;
  int boxDownY = 0;
  int boxDownX = 0;
  if (boxFromMega == 1)
  {
    boxDownX = box1down[0];
    boxDownY = box1down[1];
  }else {
    boxDownX = box2down[0];
    boxDownY = box2down[1];    
  }
  moveTo(storage[0],storage[1],storageZ);  //Move to Box 1
  delay(1000);
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  Serial.println(boxFromMega);
  while (getTip()) {
    storageZ = storageZ - 1;
    everyZ = everyZ + 1;
    if( everyZ == 3){ //This is compensating the movement of the UArm because it doesn't move Straight up and down from this low of a height. In other words it tries to keep the nozzle straight up and down.
     if(storageY > boxDownY) storageY = storageY - 1;
     if (boxFromMega == 1 )/*&& storageX > boxDownX)*/ storageX = storageX -1.5;
      everyZ = 0;
    }
    moveTo(storageX,storageY,storageZ, 25);  //Move to Box 1
    //Serial.println("@1"); // report ready
    //reportPos(); //Serial Prints the current position
    delay(10);
  }
  delay(1000);
  pumpOn();
  reportPos();
  delay(1000);
  moveTo(storage[0],storage[1], 250, 100);
  delay(1000);
  //moveTo(processing[0], processing[1], processing[2], 100); //To the cutting Modules
  moveTo(diceBox[0],diceBox[1],diceBox[2],100);
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
