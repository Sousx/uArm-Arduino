/* Sender Test Code for the mega to talk with Uno
    in order to let Uno know which process and which box
    to attempt to grab the produce.
*/
// needed for the Motors
#include <SPI.h>
// L9958 slave select pins for SPI
#define SS_M4 14
#define SS_M3 13
#define SS_M2 12
#define SS_M1 11
// L9958 DIRection pins
#define DIR_M1 2
#define DIR_M2 3
#define DIR_M3 4
#define DIR_M4 7
// L9958 PWM pins
#define PWM_M1 9
#define PWM_M2 10    // Timer1
#define PWM_M3 5
#define PWM_M4 6     // Timer0
// L9958 Enable for all 4 motors
#define ENABLE_MOTORS 8

int pwm1, pwm2, pwm3, pwm4;
boolean dir1, dir2, dir3, dir4;
bool inputted = false;

//needed for the sending string
char msg_byte = 0;
String msg_str = ""; //to build a string to send to UNO
bool newData = false;
const byte charNum = 32;
char cutFromMega[charNum] = {0};
void setup() {
  unsigned int configWord;

  // put your setup code here, to run once:
  pinMode(SS_M1, OUTPUT); digitalWrite(SS_M1, LOW);  // HIGH = not selected
  pinMode(SS_M2, OUTPUT); digitalWrite(SS_M2, LOW);
  pinMode(SS_M3, OUTPUT); digitalWrite(SS_M3, LOW);
  pinMode(SS_M4, OUTPUT); digitalWrite(SS_M4, LOW);

  // L9958 DIRection pins
  pinMode(DIR_M1, OUTPUT);
  pinMode(DIR_M2, OUTPUT);
  pinMode(DIR_M3, OUTPUT);
  pinMode(DIR_M4, OUTPUT);

  // L9958 PWM pins
  pinMode(PWM_M1, OUTPUT);  digitalWrite(PWM_M1, LOW);
  pinMode(PWM_M2, OUTPUT);  digitalWrite(PWM_M2, LOW);    // Timer1
  pinMode(PWM_M3, OUTPUT);  digitalWrite(PWM_M3, LOW);
  pinMode(PWM_M4, OUTPUT);  digitalWrite(PWM_M4, LOW);    // Timer0

  // L9958 Enable for all 4 motors
  pinMode(ENABLE_MOTORS, OUTPUT);
  digitalWrite(ENABLE_MOTORS, HIGH);  // HIGH = disabled

  /******* Set up L9958 chips *********
    ' L9958 Config Register
    ' Bit
    '0 - RES
    '1 - DR - reset
    '2 - CL_1 - curr limit
    '3 - CL_2 - curr_limit
    '4 - RES
    '5 - RES
    '6 - RES
    '7 - RES
    '8 - VSR - voltage slew rate (1 enables slew limit, 0 disables)
    '9 - ISR - current slew rate (1 enables slew limit, 0 disables)
    '10 - ISR_DIS - current slew disable
    '11 - OL_ON - open load enable
    '12 - RES
    '13 - RES
    '14 - 0 - always zero
    '15 - 0 - always zero
  */  // set to max current limit and disable ISR slew limiting
  configWord = 0b0000010000001100;

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE1);  // clock pol = low, phase = high

  // Motor 1
  digitalWrite(SS_M1, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M1, HIGH);
  // Motor 2
  digitalWrite(SS_M2, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M2, HIGH);
  // Motor 3
  digitalWrite(SS_M3, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M3, HIGH);
  // Motor 4
  digitalWrite(SS_M4, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M4, HIGH);

  //Set initial actuator settings to pull at 0 speed for safety
  dir1 = 0; dir2 = 0; dir3 = 0; dir4 = 0; // Set direction
  pwm1 = 0; pwm2 = 0; pwm3 = 0; pwm4 = 0; // Set speed (0-255)

  digitalWrite(ENABLE_MOTORS, LOW);// LOW = enabled

  Serial.begin(9600);
  Serial.println("please send an input for the UNO with the format <processingChar,boxNumber>!");
}

void loop() {
  //Serial.print(Serial.peek());
  //delay(500);
  while(Serial.available() == 0){}
  while ((Serial.available() > 0) || inputted == false) {
    //Serial.println(Serial.peek());
    msg_byte = Serial.read();
    //Serial.println(Serial.peek());
    delay(5);
    //Serial.println(Serial.peek());
    if (msg_byte != '\n') {
      msg_str += msg_byte;
      Serial.write(msg_byte);
      //delay(500);
    }
    if (Serial.peek() == -1) {
      Serial.println("");
      Serial.print("you inputted: ");
      Serial.println(msg_str);
      msg_str = "";
      msg_byte = 0;
      Serial.println("");
      Serial.println("please send an input for the UNO with the format <processingChar,boxNumber>");
      inputted = true;
    }
  }

  while (Serial.available() == 0) {}
  char rcv = Serial.read();
  strcpy(cutFromMega, rcv); //Lets Mega know we're ready to turn on the actuators
  Serial.print("Input from Uno: ");
  if (rcv == 'd') {
    //    delay(1000);
    //    //move actuator 1 in then out
    //    dir1 = 1;
    //    pwm1 = 255; //set direction and speed
    //    digitalWrite(DIR_M1, dir1);
    //    analogWrite(PWM_M1, pwm1); // write to pins
    //    delay(5000);
    //    dir1 = 0;
    //    pwm1 = 255; //set direction and speed
    //    digitalWrite(DIR_M1, dir1);
    //    analogWrite(PWM_M1, pwm1); // write to pins
    //    delay(5000); //allows enough time for actuator to retract
    Serial.println("I made it in hereD!");
  } else if (rcv == 's') {
    //    delay(1000);
    //    dir3 = 1;
    //    pwm3 = 255;
    //    digitalWrite(DIR_M3, dir3);
    //    analogWrite(PWM_M3, pwm3);
    //    delay(5000);
    //    dir3 = 0;
    //    pwm3 = 255;
    //    digitalWrite(DIR_M3, dir3);
    //    analogWrite(PWM_M3, pwm3);
    //    delay(5000);
    Serial.println("I made it in hereS!");
  } else if (rcv == 'f') {
    Serial.print("Incorrect input from Uno, recieved: ");
  }
  Serial.println(rcv);
  inputted = false;
}


