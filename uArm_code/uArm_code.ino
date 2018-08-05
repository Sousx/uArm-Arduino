
/*Code for Uarm Arduino Uno movement and receiving input from Mega
 * 
 * 
 */

#include "uArm.h"
#include "uArmAPI.h"
#include "uArm_code.h"

unsigned long tickStartTime = millis(); // get timestamp;
static void Init();

int box1[] = {4, 130, 150};
int box2[] = {78, 126, 161};

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

int boxFromMega = 0;

boolean newData = false;

void setup()
{

  Serial.begin(9600);
  //Serial.println("Started");

  //1. First Test is a simple run of serial communication
  
  //testSerialUno();

  //2. Testing if a command from the uArm is interpreted by the Mega to run the actuator

  configureUArm();//this goes to loop to execute commands made by team before
  
}

void loop(){
  //while(!Serial.available()) {
  //}
  //Serial.println(Serial.read());
  run(); // Don't remove
  recvWithStartEndMarkers();
 
   
  if (newData == true) {
    //Serial.println("Here");
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();

    if (boxFromMega == 1){
      //Serial.println("Get food from box 1");
      getFood(box1);
      Serial.write("d");
    }else if (boxFromMega == 2){
      //Serial.println("Get food from box 2");
      getFood(box2);
      Serial.write("d");
    } else {
      //Serial.println("You didn't pick a food box, jackass.");
    }
    //Serial.println("Please input <1> or <2>");
    newData = false;
  }
}


void testSerialUno() {
  //char* str;
  int i = 0;
  while(!Serial.available()) {
  }
  String stringIn = Serial.readString();
  //str = stringIn.toCharArray();
  delay(100);
  while(stringIn.charAt(i) != '\0') {
    Serial.write(stringIn.charAt(i));
    i++;
  }
}

void configureUArm() {
    
  Init(); // Don't remove
   //TODO
  moveTo(0, 150, 150, 50); //Neutral Position coordinates
  //Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  //Serial.println("Please input <1> or <2>");
}

void getFood(int storage[])
{
  //delay(1000);
  int storageZ = storage[2];
  moveTo(storage[0],storage[1],storageZ);  //Move to Box 
  delay(1000);
  //Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  //Serial.println(boxFromMega);
  while (getTip()) {
    storageZ = storageZ - 1;
    moveTo(storage[0],storage[1],storageZ, 25);  //Move to Box
    delay(10);
  }
  delay(1000);
  pumpOn();
  reportPos();
  delay(1000);
  moveTo(storage[0],storage[1], 200, 100);
  delay(1000);
  moveTo(-116,223,194, 100);
  pumpOff();
  delay(1000);
  moveTo(0, 150, 150); //Neutral Position coordinates
  //Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  
  if(!Serial.available()) {  
    Serial.write("r");
    delay(1000);
    //Serial.flush();
  } //do nothing until input
  delay(5);
  while ((Serial.available() > 0) && newData == false){
    rc = Serial.read();
    //Serial.println(rc);
    if (recvInProgress == true) {
      //Serial.println("Got Here");
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        //Serial.println("End");
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      //Serial.println("Start");
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
    //Serial.println("Box from Mega: ");
    //Serial.println(boxFromMega);
    //strtokIndx = strtok(NULL, ",");
    //floatFromPC = atof(strtokIndx);     // convert this part to a float

}

//============

void showParsedData() {
    //Serial.print("Food from box: ");
    //Serial.println(boxFromMega);
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
