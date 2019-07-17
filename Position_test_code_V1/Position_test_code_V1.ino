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


//Use the jumpers on the board to select which pins will be used
int EnablePin1 = 13;
int PWMPinA1 = 11;
int PWMPinB1 = 3;

void goToHere(int destination);
void slowDown(boolean pushing, int rate);
int velocity();

void(* resetFunc) (void) = 0; //declare reset function @ address 0

int startPos = 65;
int endPos = 878;

const int actuatorInput = A0;
//const int precision = 2; //set as 
//const int default_sequence = 12121;
int pos = 0;
int currentPos = 0;
int actuatorSpeed = 0;

int previousPos = 0; // previous iteration of "pos" during push or pull
int previousTime = 0; // previous iteration of "time" during push or pull
int slowedIntervals = 0; // shows how many iterations the actuator has been slow

void setup() {  
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Started");
  
  configureActuator(); // set up actuator
  resetPosition();
  
}
void loop() {

  test1();
  test2();
  for(;;);  

}//end main loop

void test1() { // two cycles
   forwardAndBack();
   forwardAndBack();    
}

void test2() { // test slow down
  slowDown(true, 5, 0); // slow down an actuator moving forward to stop
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
  //checkSlowed();
  analogWrite(PWMPinA1, actuatorSpeed); //changed to 50 for test speed
  analogWrite(PWMPinB1, 0); // speed 0-255
}

void pullActuator() {
  if(Serial.read() == 'p') {
    pauseActuator();
  }
  //checkSlowed();
  Serial.println("Here");
  Serial.println(actuatorSpeed);
  analogWrite(PWMPinA1, 0);
  analogWrite(PWMPinB1, actuatorSpeed);//speed 0-255 //changed to 50 for test speed
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
    difference = destination - currentPos;
  }
  else {
   Serial.println("We need to pull");
   difference = currentPos - destination;
  }
  Serial.println("This much: ");
  
  Serial.println(difference);
  while(difference > 0) {
   if(Serial.read() == "stop") {
    stopActuator();
    Serial.print("Actuator has been stoppped");
    break;
   }
   currentPos = analogRead(actuatorInput);
   Serial.println("We are now here");
   Serial.println(currentPos);
   Serial.println(difference);
   actuatorSpeed = 255;
    if(pushing) {
      difference = destination - currentPos;
      pushActuator();
    }
    else {
      difference = currentPos - destination;
      pullActuator();
    }
  }
  delay(50);
  stopActuator();
 Serial.println("We have reached our destination");
}

void slowDown(boolean pushing, int rate, int finalSpeed) {
  if(actuatorSpeed == 0) {
    Serial.println("The actuator is not moving");
  }
  if(actuatorSpeed == finalSpeed) {
    Serial.println("The actuator is already going at the intended speed");
  }
  while(actuatorSpeed != finalSpeed) { 
    if(pushing) {
      pushActuator();
    }
    else {
      pullActuator();
    }
    actuatorSpeed -= rate; // change based on intended rate of change in velocity
    delay(50);
  }
}

int velocity() {
  Serial.print("Velocity is: ");
  Serial.println((pos - previousPos) / (millis() - previousTime));
  return (pos - previousPos) / (millis() - previousTime);
}

boolean checkSlowed() {
  int slowSpeed; // TODO: test speed of actuator to determine threshold of speed indicating that the actuator has slowed down
                 // (i.e. this is the actual derivative)

  if(slowedIntervals > 5) { // If the actuator has been slow for this long
    Serial.println("Actuator is having trouble cutting this");
    resetPosition();
    return true;
  }
  if(velocity() <= slowSpeed) { //If the actuator is slower than this speed
    slowedIntervals++; // Increment time recorded that the actuator is slowed down
  }
  else {
    slowedIntervals = 0;
  }
  return false;
}
