/*  This code modifies the MegaMoto example code to control an actuator with
  no potentiometer.
  The actuator will extend for 10 seconds, and immediately retract for 10 seconds.
  This process repeats over 300 seconds.
  Modified by Progressive Automations, using the original example code 
  for the MegaMoto board from:
  <a href="http://www.robotpower.com/downloads/" rel="nofollow"> http://www.robotpower.com/downloads/</a>

  Hardware:
  - 1 RobotPower MegaMoto control board
  - Arduino Uno
  
  Wiring:
  -Connect the +/- of the actuator to the A/B motor channels
  -Connect the +/- of the power supply to the +/- BAT connections
  This example code is in the public domain.
 */
#define SUCCESS 1
#define FAILURE 0
#define WORD 1
#define UARM_TIMEOUT 15000 //CHANGE THIS BASED ON HOW LONG IT WILL TAKE UARM TO COMPLETE ITS JOB

#define touchPin 22
#define ZERO 0
#define ONE 1
#define TWO 2
#define SERIES 3

#define TOUCH_TIMEOUT 5000

#define SERIAL_MODE 1
#define TOUCH_SENSOR_MODE 2

//Use the jumpers on the board to select which pins will be used
int EnablePin1 = 13;
int PWMPinA1 = 11;
int PWMPinB1 = 3;

int mode = ZERO; 

String waitForUnoCommand();
void goToHere(int destination);

void(* resetFunc) (void) = 0; //declare reset function @ address 0

int startPos = 347;
int endPos = 610;

const int actuatorInput = A0;
const int precision = 2; //set as 
const int default_sequence = 12121;
int pos = 0;

void setup() {  
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Started");
  //1. First Test is a simple run of serial communication
  
  //testSerialMega();


  //2. Testing if a command from the uArm is interpreted by the Mega to run the actuator
  pinMode(touchPin, INPUT);
  mode = ZERO;
  
  configureActuator(); // set up actuator
  resetPosition();
  
}
void loop() {
  uint8_t userInput;
  int finalInput;
  int mode = TOUCH_SENSOR_MODE;
  if(mode == SERIAL_MODE) {
    userInput = serialInputMode();
  }
  else {
    userInput = touchSensorInputMode();
  }
  if(userInput > 3) {
    userInput = 0;
  }
//  if(userInput == 's') {
//    Serial.println("Serial
//    userInput = seriesMode();
//  }
  Serial.println("User Input");
  Serial.println(userInput);
//  if(userInput == 0) { //if a selection wasn't put in
//    Serial.println("Setting Default Sequence");
//    finalInput = default_sequence;
//  }
//  else {
  finalInput = (int) userInput;
  if(userInput == 3) {
    finalInput = default_sequence;
  }
  //}
  Serial.println("Sequence is: ");
  Serial.println(finalInput);
  char inputChar;
  while(finalInput % 10 != 0) {
    Serial.println("Current Digit: ");
    Serial.println(finalInput % 10);
    inputChar = (finalInput % 10) + '0';
    Serial.println(inputChar);
    if(uArmPickup(inputChar) == SUCCESS) { //if uArm gets produce to aisle for cutting
      forwardAndBack(); //cut it
    }
    finalInput /= 10;
  }
}//end main loop

//  uint8_t seriesMode() {
//    Serial.println("Input the sequence of boxes you want to pick up from");
//    while(!Serial.available()){}
//    uint8_t sequence = Serial.read();
//    return sequence;
//  }

  int serialInputMode() {
    int userInput= 0;
    Serial.println("SELECTIONS:");
    Serial.println("1. Select one box to pick up produce from (1 or 2)");
    Serial.println("2. Type 's' to initiate 'series mode'");
    //Serial.println("3. Wait 10 seconds to perform default series test (1,2,1,2,1)");
    inputTimeOut(10); //give user 10 seconds to put something in
    userInput = Serial.read();    
  }

  int touchSensorInputMode() {
    int userInput = 0;
    int timeLow = 0; //default
    Serial.println("SELECTIONS:");
    Serial.println("To count a press on the actuator, hold for at least one second (but not over three)");
    Serial.println("1. Press once to grab food from box 1, or twice to grab from box 2.");
    Serial.println("2. Hold for more than three seconds to initiate a default sequence test of (1,2,1,2,1)");
    while(timeLow != 5) { // while the button hasn't been pressed and a choice hasn't been made
      int timeHigh = 0;
      Serial.print("Time low at loop: ");
      Serial.println(timeLow);
      Serial.print("user input at loop: ");
      Serial.println(userInput);
      unsigned long timeStart, timeNow = 0;
      timeStart = millis();
      timeNow = millis();
      Serial.println("Here");
      while(timeHigh == 0 && (timeNow - timeStart < TOUCH_TIMEOUT)) { // wait for a press if none, for up to 5 seconds
        Serial.println("Waiting for press...");
        timeHigh = timeInState(HIGH);
        timeNow = millis();
      }
      Serial.print("Time held high: ");
      Serial.println(timeHigh);
      timeLow = timeInState(LOW);
      Serial.print("Time held low: ");
      Serial.println(timeLow);
      if(timeHigh > 2) {
        Serial.println("held..entering series mode");
        userInput = 3;
      }
      else if(timeHigh > 0) {  
        if (userInput == 2) {
          Serial.println("pushed too many times");
        }
        else if(userInput == 1) {
          userInput = 2; 
          if(timeLow > 1) {
            Serial.println("Pressed twice");
          }
        }
        else { // mode == ZERO
          userInput = 1; 
          if(timeLow > 1) {
            Serial.println("Pressed once");
          }
        }
      }
      else {
        //timeLow = timeInState(LOW);
        //Serial.println("inactive");
      }
    }
      Serial.print("Time low at loop: ");
      Serial.println(timeLow);
      Serial.print("user input at loop: ");
      Serial.println(userInput);
    delay(350);
    return userInput;
  }

  int timeInState(int state) {
    int timeHeld = 0;
    unsigned long timeNow, timeStart = 0;
    bool timeOut = false;
    timeStart = millis();
    timeNow = millis();
    Serial.print("Check touch state: ");
    Serial.println(checkTouch());
    Serial.print("State chosen: ");
    Serial.println(state);
    while((checkTouch() == state) && timeOut == false){ // check if the state of the touch sensor reflects
                                                        // what you want it to stay at (e.g. off if you want it off)
      timeNow = millis();
      if(timeNow - timeStart > TOUCH_TIMEOUT) {
        Serial.println("held state for too long");
        return 5;
      }
      if(state == HIGH) {
        Serial.println("still high");
      }
      else {
        Serial.println("still low");
      }
    }
    timeHeld = (timeNow - timeStart) / 1000;
    Serial.print("Time Held: ");
    Serial.println(timeHeld);
    if(timeHeld == 0) {
      Serial.println("Let go too early");
    }
    return timeHeld;
  }
  int checkTouch() {
    int touchValue = digitalRead(touchPin);
    if (touchValue == HIGH) {
      Serial.println("TOUCHED");
      //digitalWrite(ledPin, HIGH);
    } else {
      //digitalWrite(ledPin, LOW);
    }
    return touchValue;
  }


  void inputTimeOut(unsigned int seconds) {
    unsigned long timeNow, timeStart = 0;
    bool timeOut = false;
    timeStart = millis();
    timeNow = millis();
    while(!Serial.available() && timeOut == false){
      if(timeNow - timeStart < seconds * 1000) {
        timeNow = millis();
      }
      else {
        timeOut = true;
        Serial.println("Timed out");
      }
    }
  }
  
void resetPosition() {
  goToHere(startPos);
  Serial.println("Actuator Ready To Go!");
}

void testSerialMega() {
  String stringBack;
  Serial1.write("hello uarm");
  stringBack = waitForUnoCommand(/*WORD*/);
 Serial.println("Word has been received");
 Serial.println(stringBack);
  //stringBack = "";
}

int uArmPickup(char boxNumber) {
  if(boxNumber != '1' && boxNumber != '2') { // input data validation
   Serial.println("invalid box number..please input 1 or 2");
  }
  else {
    if(waitForUnoCommand(/*!WORD*/) != "r") {
      Serial.println("Did not receive character indicating that the uArm is ready");
      return FAILURE;
    }
    Serial.println("Received input, setting request...");
    int i = 0;
    Serial.println("Done");
    Serial1.write("<"); // send command body with number
    Serial1.write(boxNumber);
    Serial1.write(">");
    if(waitForUnoCommand(/*!WORD*/) == "d") { 
      return SUCCESS; //uArm is successful!
    }
    else {
     Serial.println("invalid input from uArm (MAY HAVE TIMED OUT)");
      return FAILURE;
    }
  }
}

void inputBufferFlush() {
  while(Serial1.available()) {
    Serial.print("Characters eliminated");
    Serial.print(Serial1.read());
  }
}

void configureActuator() {
  pinMode(EnablePin1, OUTPUT);//Enable the board
  pinMode(PWMPinA1, OUTPUT);
  pinMode(PWMPinB1, OUTPUT);//Set motor outputs
  pinMode(actuatorInput, INPUT);
  digitalWrite(EnablePin1, HIGH); // enable the motor
}

void forwardAndBack() {
  pos = analogRead(actuatorInput); //initial position recorded by pot
 Serial.println("Intital Position is: ");
 Serial.println(pos);
  goToHere(endPos);
  delay(50);
  goToHere(startPos);
  delay(50);
}

String waitForUnoCommand(/*int wordInput*/) {
  String stringIn;
  unsigned long timeNow, timeStart = 0;
//  while(!Serial1.available()) {
//   Serial.println("Waiting for comms. input");
//  }
  //while(!Serial1.available()) {
  //}
  timeStart = millis();
  timeNow = millis();
  Serial.println("Time start in milliseconds is: ");
  Serial.println(timeStart);
  inputBufferFlush();
  while(!Serial1.available()) { //wait for uno to send input
    Serial.println("Time elapsed in milliseconds is: ");
    Serial.println(timeNow - timeStart);
    if(timeNow - timeStart > UARM_TIMEOUT) {
      Serial.println("Surpassed timeout for uArm");
      return "";
    }
    else {
      Serial.println("Waiting for comms. input");
      timeNow = millis();
    }
  }
  //while(Serial1.available()) { //input has arrived
  Serial.println("Received Input");
   //if(wordInput) {
  char charIn = Serial1.read();
  stringIn.concat(charIn); //input arrives one char at a time, will be appended to string (may not be literal, if it isnt, then char*)
   //}
//   else {
//    String charIn = Serial1.read();
//    Serial.println(charIn);
//    //Serial.println(Serial1.read());
//    Serial.println("Only one letter");
//    return charIn;
//   }
//  }
  Serial.println(charIn);
  Serial.println(stringIn);
  return stringIn;
}

void stopActuator() {
  analogWrite(PWMPinA1, 0);
  analogWrite(PWMPinB1, 0); // speed 0-255
}

void pauseActuator() {
  stopActuator();
  while(Serial.peek() == 'x' || Serial.read() != 's') {
    if(Serial.read() == 'x') {
      Serial.println("Resetting...");
      resetSystem();
    }
    Serial.println("Paused");
  }
}

void resetSystem() {
  Serial.write('x'); //write to uno to make it reset
  resetFunc();
}

void pushActuator() {
  if(Serial.read() == 'p') {
    pauseActuator();
  }
  analogWrite(PWMPinA1, 255);
  analogWrite(PWMPinB1, 0); // speed 0-255
}

void pullActuator() {
  if(Serial.read() == 'p') {
    pauseActuator();
  }
  analogWrite(PWMPinA1, 0);
  analogWrite(PWMPinB1, 255);//speed 0-255
}

void goToHere(int destination) {
  bool pushing = false;
  int difference = 0;
  int currentPos = analogRead(actuatorInput);
  Serial.print("Destination is: ");
  Serial.println(destination);
  Serial.print("Actuator is at: ");
  Serial.println(currentPos);  
  if(destination > currentPos) {
   Serial.println("We need to push");
    pushing = true;
  }
  else {
   Serial.println("We need to pull");
  }
  Serial.println("This much: ");
  difference = destination - currentPos;
  Serial.println(difference);
  while(difference > precision || difference < -precision) {
   if(Serial.read() == "stop") {
    stopActuator();
    Serial.print("Actuator has been stoppped");
    break;
   }
   currentPos = analogRead(actuatorInput);
   Serial.println("We are now here");
   Serial.println(currentPos);
    difference = destination - currentPos;
    if(pushing)
      pushActuator();
    else
      pullActuator();
  }
  delay(50);
  stopActuator();
 Serial.println("We have reached our destination");
}
