#define touchPin 22
#define ZERO 0
#define ONE 1
#define TWO 2
#define SERIES 3

#define FAILURE 0
#define SUCCESS 1

#define TOUCH_TIMEOUT 5000
// Uncomment the code relating to 'ledPin' if you do not want to LED to flash
 
int ledPin = 13;
int mode = ZERO;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(touchPin, INPUT);
  mode = ZERO;
}
 
void loop() {
  int timeHigh = timeInState(HIGH);
  Serial.print("Time held high: ");
  Serial.println(timeHigh);
  if(timeHigh > 2) {
    Serial.println("held..entering series mode");
    mode == SERIES;
  }
  else if(timeHigh > 0) {  
    int timeLow = timeInState(LOW);
    Serial.print("Time held low: ");
    Serial.println(timeLow);
    if (mode == TWO) {
      Serial.println("pushed too many times");
    }
    else if(mode == ONE) {
      mode = TWO; 
      if(timeLow > 1) {
        Serial.println("Pressed twice");
      }
    }
    else { // mode == ZERO
      mode = ONE; 
      if(timeLow > 1) {
        Serial.println("Pressed once");
      }
    }
  }
  else {
    Serial.println("inactive");
  }
  delay(350);
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

