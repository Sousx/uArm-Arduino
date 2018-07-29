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
#define UARM_TIMEOUT 500000 //CHANGE THIS BASED ON HOW LONG IT WILL TAKE UARM TO COMPLETE ITS JOB

//Use the jumpers on the board to select which pins will be used
int EnablePin1 = 13;
int PWMPinA1 = 11;
int PWMPinB1 = 3;

String waitForSerialCommand();
void goToHere(int destination);


int startPos = 347;
int endPos = 580;

const int actuatorInput = A0;
const int precision = 2; //set as desired
int pos = 0;

void setup() {  
  Serial.begin(9600);
  Serial1.begin(9600);
 Serial.println("Started");
  //1. First Test is a simple run of serial communication
  
  //testSerialMega();


  //2. Testing if a command from the uArm is interpreted by the Mega to run the actuator

  configureActuator(); // set up actuator
  resetPosition();
}
void loop() {
  Serial.println("Which box would you like produce from? (1 or 2)");
  while(!Serial.available())
  {
  }
  uint8_t userInput = Serial.read();
  if(uArmPickup(userInput) == SUCCESS) { //if uArm gets produce to aisle for cutting
    forwardAndBack(); //cut it
  }
}//end main loop

void resetPosition() {
  goToHere(startPos);
  Serial.println("Actuator Ready To Go!");
}
void testSerialMega() {
  String stringBack;
  Serial1.write("hello uarm");
  stringBack = waitForSerialCommand(/*WORD*/);
 Serial.println("Word has been received");
 Serial.println(stringBack);
  //stringBack = "";
}

int uArmPickup(char boxNumber) {
  if(boxNumber != '1' && boxNumber != '2') { // input data validation
   Serial.print("invalid box number..please input 1 or 2");
  }
  else {
    if(waitForSerialCommand(/*!WORD*/) != "r") {
      Serial.print("Did not receive character indicating that the uArm is ready");
      return FAILURE;
    }
    Serial.print("Received input, setting request...");
    int i = 0;
    Serial.println("Done");
    Serial1.write("<"); // send command body with number
    Serial1.write(boxNumber);
    Serial1.write(">");
    if(waitForSerialCommand(/*!WORD*/) == "d") { 
      return SUCCESS; //uArm is successful!
    }
    else {
     Serial.println("invalid input from uArm (MAY HAVE TIMED OUT");
      return FAILURE;
    }
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

String waitForSerialCommand(/*int wordInput*/) {
  String stringIn;
  unsigned long timeNow, timeStart = 0;
  while(!Serial1.available()) {
   Serial.println("Waiting for comms. input");
  }
  //while(!Serial1.available()) {
  //}
  timeStart = millis();
 Serial.println("Time start in milliseconds is: ");
 Serial.println(timeStart);
  while(!Serial1.available()) { //wait for uno to send input
    Serial.println("Time now in milliseconds is: ");
    Serial.println(timeNow);
    if(timeNow - timeStart > UARM_TIMEOUT) {
     Serial.println("Surpassed timeout for uArm");
      return "";
    }
    else {
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

void pushActuator() {
  analogWrite(PWMPinA1, 255);
  analogWrite(PWMPinB1, 0); // speed 0-255
}

void pullActuator() {
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
