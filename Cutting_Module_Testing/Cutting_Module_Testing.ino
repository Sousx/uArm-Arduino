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
 
//Use the jumpers on the board to select which pins will be used
int EnablePin1 = 13;
int PWMPinA1 = 11;
int PWMPinB1 = 3;


int extendtime = 10 * 1000;  // 10 seconds, times 1000 to convert to milliseconds
int retracttime = 10 * 1000; // 10 seconds, times 1000 to convert to milliseconds
int timetorun = 300 * 1000; // 300 seconds, times 1000 to convert to milliseconds


int duty;
int elapsedTime;
boolean keepMoving;


void setup() {
  Serial.begin(9600);
  pinMode(EnablePin1, OUTPUT);//Enable the board
  pinMode(PWMPinA1, OUTPUT);
  pinMode(PWMPinB1, OUTPUT);//Set motor outputs
  elapsedTime = 0; // Set time to 0
  keepMoving = true; //The system will move
  
}//end setup
void loop() {
  if (keepMoving) 
  {
    digitalWrite(EnablePin1, HIGH); // enable the motor
    pushActuator();
    delay(extendtime);
    stopActuator();
    delay(10);//small delay before retracting
    
    pullActuator();
    delay(retracttime);
    stopActuator();
    
    elapsedTime = millis();//how long has it been?
    if (elapsedTime > timetorun) {//if it's been 300 seconds, stop

      Serial.print("Elapsed time is over max run time. Max run time: ");
      Serial.println(timetorun);
      keepMoving = false;  
    }
  }//end if
}//end main loop


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
