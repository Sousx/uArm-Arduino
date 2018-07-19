/* Testing for the receiving a string to parse and process to let the Uno know which and what to do with produce
 *  String must be sent in the format of <boxNumber,Process> where boxNumber is 1 or 2, and Process 
 */


#include "uArm.h"
//#define buttonState
#define USE_SERIAL_CMD  1 // 1: use serial for control  0: just use arduino to control(release ROM and RAM space)
unsigned long tickStartTime = millis(); // get timestamp;
static void Init();
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
char messageFromMega[numChars] = {0};
int integerFromMega = 0;
boolean newData = false;

void setup() {
       // Wire.begin();      // join i2c bus (address optional for master)
        Serial.begin(9600); // start serial port at 9600 bps
       // uArmInit();
       // pinMode(2,INPUT);
       Init(); // Don't remove
        moveTo(0,150,150,50);
       //Serial.println("@1");
       //Serial.println("please input <processChar,boxNumber>");
       
}

void loop() {
        run();
        
        recvWithStartEndMarkers();
        if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        showParsedData();
        newData = false;
       //Serial.println("please input <processChar,boxNumber>");
    }
      

}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
    //Serial.println("please input <processChar,boxNumber>");
    while(Serial.available() == 0){}
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//======================
void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(messageFromMega, strtokIndx); // copy it to messageFromMega
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    integerFromMega = atoi(strtokIndx);     // convert this part to an integer

//    strtokIndx = strtok(NULL, ",");
//    floatFromPC = atof(strtokIndx);     // convert this part to a float

}

//============

void showParsedData() {
   //Serial.print("I am ");
    //char slicing[numChars] = {'s','\0'};
    //char dicing[numChars] = {'d','\0'};
    if (strcmp(messageFromMega,"s") == 0){
     //Serial.print("slicing ");
    } else if (strcmp(messageFromMega,"d") == 0){
     //Serial.print("dicing ");
    }else{
     //Serial.print("not sure what I am doing with ");
    }
   //Serial.print("the produce. You inputted: ");
   //Serial.println(messageFromMega);
    if (integerFromMega < 3){
     //Serial.print("I am getting the produce from box ");
    } else {
     //Serial.print("I am not sure where to get the produce from. You inputted: ");
    }
   //Serial.println(integerFromMega);
//   //Serial.print("Float ");
//   //Serial.println(floatFromPC);
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
