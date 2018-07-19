//This example code uses our LC-80, LC-066, any linear actuator and a power source. You can get more detail on the code and what it does in our blog post.
//Use the jumpers on the board to select which pins will be used
int EnablePin1 = 13;
int PWMPinA1 = 11;
int PWMPinB1 = 3;
unsigned long extendtime = 10 * 1000;  // 10 seconds, times 1000 to convert to milliseconds
unsigned long retracttime = 9 * 1000; // 10 seconds, times 1000 to convert to milliseconds
unsigned int timetorun = 60 * 1000; // 60 seconds, times 1000 to convert to milliseconds
//unsigned long test = long(60) * (1000);
int duty;
unsigned long elapsedTime;
boolean keepMoving;
void setup() {
  Serial.begin(9600);
  pinMode(EnablePin1, OUTPUT);//Enable the board
  pinMode(PWMPinA1, OUTPUT);
  pinMode(PWMPinB1, OUTPUT);//Set motor outputs
  elapsedTime = 0; // Set time to 0
  digitalWrite(EnablePin1, HIGH);
  pullActuator();
  delay(5000);
  stopActuator();
  keepMoving = true; //The system will move
  
}//end setup
void loop() {
 if (keepMoving) 
  {
    digitalWrite(EnablePin1, HIGH); // enable the motor
    pushActuator();
    delay(extendtime);
    stopActuator();
    delay(50);//small delay before retracting
    
    pullActuator();
    delay(retracttime);
    stopActuator();
    delay(50);
    
    elapsedTime = millis();//how long has it been?
//    Serial.println(test);
    Serial.println(elapsedTime);
//    Serial.println(timetorun);
    if (elapsedTime > timetorun) {//if it's been 60 seconds, stop
      Serial.print("Elapsed time is over max run time. Max run time: ");
      Serial.println(timetorun);
     //Serial.println(60);
      keepMoving = false;  
    }
  }//end if*/
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
