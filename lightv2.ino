#include <IRremote.h>

// SENSOR PINS
int irPin = 4;
int hallPin = A7;
int sButton = 8;
int sBuzzer = 3;
int mainButton = 10;

int lightState = 0;

// LED PINS
int yLed = 5;
int gLed = 6;
int rLed = 7;

int gBlink = 500;
int gDuration = 15000;
int gState = 1;

int ledState = 0;

// IR DECODER
IRrecv irrecv(irPin);
decode_results results;
int irPress = 0;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  pinMode(irPin, INPUT_PULLUP);
  pinMode(hallPin, INPUT_PULLUP);
  pinMode(sButton, INPUT_PULLUP);
  pinMode(mainButton, INPUT_PULLUP);
  
  pinMode(sBuzzer, OUTPUT);
  pinMode(rLed, OUTPUT);
  pinMode(gLed, OUTPUT);
  pinMode(yLed, OUTPUT);

  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
}

void loop() {
  int hallRead = readHallSensor();
  int irPress = decodeIr();
  int mainInput = digitalRead(mainButton);
  if (mainInput == 0) {
    if (ledState == 1) {
      ledState = 2;
      gDuration = 15000;
      gBlink = 500;
      gState = 1;
      Serial.println("GREEN INPUT");
      delay(200);
    } else {
      ledState = 1;
      Serial.println("RED INPUT");
      delay(200);
    }
  }

  int light = analogRead(A6);

  if (lightState == 0 && light > 900) {
    lightState = 1;
    playMorningSong();
  } else if (light <= 900) {
    lightState = 0;
  }

  // led verte allumée
  if ((irPress == 1 || gDuration > 0) && ledState != 1) {
    if (gState == 1) {
      digitalWrite(gLed, HIGH);
      digitalWrite(rLed, LOW);
    } else {
      digitalWrite(gLed, LOW);
      digitalWrite(rLed, LOW);
    }

    gDuration -= 50;
    gBlink -= 50;
    delay(50);

    if (gDuration <= 0) {
      ledState = 1;
      gDuration = 0;
      gState = 1;
      gBlink = 500;
      Serial.println("GREEN END");
    }

    else if (gDuration <= 4000 && gBlink <= 0) {
      if (gState == 0) {
        gState = 1;
      } else {
        gState = 0;
      }
      gBlink = 500;
    }
  }

  // led rouge allumée
  else {
    digitalWrite(rLed, HIGH);
    digitalWrite(gLed, LOW);
    
    ledState = 1;
    gDuration = 0;
  }
  
  makeSound(hallRead);
}

int readHallSensor() {
  if (analogRead(hallPin) < 500) {
    digitalWrite(yLed, HIGH);
    return 1;
  } else {
    digitalWrite(yLed, LOW);
    return 0;
  }
}

int decodeIr(){
  int irPress = 0;
  
  if (irrecv.decode(&results))
  {
    switch(results.value)
    {
      // green
      case 0x2DD37549:
        irPress = 1;
        gDuration = 15000;
        Serial.println("UP");
        break;
        
      // red
      case 0xB4A116F2: 
        irPress = 2;
        gDuration = 0;
        Serial.println("DOWN");
        break;
    }
    
    irrecv.resume();
  }
}

void makeSound(int hallRead) {
  int sInput = digitalRead(sButton);
  
  if (ledState == 1 && hallRead == 0) {
    Serial.println("ALERT");
    for (int i = 1000; i < 2000; i++) {
      buzz(3, i, 1);
    }
  } else if (!sInput && ledState == 1) {
    Serial.println("DING DONG");
    buzz(3, 12000, 10);
    buzz(3, 6000, 20);
  }
}

void playMorningSong(){
    Serial.println("GOOD MORNING");
    for (int j = 0; j < 50; j++) {
      buzz(3, 8000, 5);
      buzz(3, 6000, 5);
    }
}

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2;
  long numCycles = frequency * length / 1000;
  for (long i = 0; i < numCycles; i++) {
    digitalWrite(targetPin, HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(targetPin, LOW);
    delayMicroseconds(delayValue);
  }
}

