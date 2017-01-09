#include <Bounce.h>

#define MAX_FUEL 10
#define MIN_FUEL 00
#define TRANSFER_RATE 600  // 1000


int bottleButton = 0;
int bottleReed =   1;
int loadSwitch =   3;
int unloadSwitch = 2;
int pumpSwitch =   4;

int latchPin =    10;
int clockPin =    13;
int dataPin =     11;


//byte barGraph[10];
byte barGraph1[11] = {
  0b00000000, 0b00000001, 0b00000011, 0b00000111, 0b00001111, 0b00011111, 0b00111111, 0b01111111, 0b11111111, 0b11111111, 0b11111111
};
byte barGraph2[11] = {
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000011
};

byte ledArray2, ledArray1;
int fuelLevel = 0;  // 0 - 10
int currentFuelInBottle, alarmFuelInBottle;

boolean greenLED, yellowLED, emptyFuelLED, alarmLED;
boolean bottleLoaded = false;
boolean bottleArmed = false;
Bounce armButton = Bounce(bottleButton, 500);
boolean loadFuel = false;
boolean unloadFuel = false;
boolean pumpOn = false;
boolean fuelReady = false;
boolean alarm = false;


void setup() {
  Serial.begin(115200);

  pinMode(bottleButton, INPUT_PULLUP);
  pinMode(bottleReed, INPUT_PULLUP);
  pinMode(loadSwitch, INPUT_PULLUP);
  pinMode(unloadSwitch, INPUT_PULLUP);
  pinMode(pumpSwitch, INPUT_PULLUP);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  yellowLED = false;
  greenLED = false;
  emptyFuelLED = false;

  if (digitalRead(bottleReed) == LOW) {
    bottleLoaded = true;
    currentFuelInBottle = MAX_FUEL;
  }
}



elapsedMillis shiftPleaseTimer;
elapsedMillis yellowBlinkTimer;
elapsedMillis blueBlinkTimer;
elapsedMillis fuelLoadTimer;
//elapsedMillis fuelTransfer;

void loop() {
  //TODO remember bottle fuel in case of alarm
  //TODO fix remember fuel for arm/disarm (when bottle not removed) - or just remember always? (see line 9 of buttonsAndSwitches + somewhere else?)
  //TODO implement audio
  
  readButtonsAndSwitches();

  if (!bottleLoaded && bottleArmed) {
    alarm = true;
    if (blueBlinkTimer >= 750) {
        blueBlinkTimer = 0;
        alarmLED = !alarmLED;
      }
    Serial.println("ALARM!");
  } else {
    alarm = false;
    alarmLED = false;
  }
  
  if (bottleArmed && !alarm) {
    yellowLED = false;

    // START fuel transfers
    if (pumpOn) {
      if (loadFuel && fuelLevel > MIN_FUEL && fuelReady) {
        if (fuelLoadTimer >= TRANSFER_RATE) {
          fuelLoadTimer = 0;
          fuelLevel--;
        }
      } else if (unloadFuel && (fuelLevel < currentFuelInBottle) && fuelReady) {
        if (fuelLoadTimer >= TRANSFER_RATE) {
          fuelLoadTimer = 0;
          fuelLevel++;
        }
      }
    }

    if (fuelLevel == MIN_FUEL) {
      if (blueBlinkTimer >= 750) {
        blueBlinkTimer = 0;
        emptyFuelLED = !emptyFuelLED;
      }
    } else {
      emptyFuelLED = false;
    }
    // END fuel transfers


    if (fuelLevel < currentFuelInBottle && !fuelReady) {
      if (fuelLoadTimer >= 100) {
        fuelLevel++;
        fuelLoadTimer = 0;
      }
      if (fuelLevel == currentFuelInBottle) {
        greenLED = true;
        fuelReady = true;
      }
    }
  } else {
    greenLED = false;
    if (!bottleLoaded) {
      yellowLED = false;
      fuelReady = false;
    } else if (!fuelReady) {
      if (yellowBlinkTimer >= 400) {
        yellowBlinkTimer = 0;
        yellowLED = !yellowLED;
      }
    }
    if (fuelLevel > 0 && !loadFuel && !unloadFuel) {
      if (alarm) {
        fuelLevel = 0;
      } else if (fuelLoadTimer >= 100) {
        fuelLevel--;
        fuelLoadTimer = 0;
      }
      if (fuelLevel == 0) {
        fuelReady = false;
      }
    }
  }

  if (yellowLED) ledArray2 |= 0b00000100;    // yellow = 0b00000100
  if (greenLED) ledArray2 |= 0b00001000;     // green = 0b00001000
  if (emptyFuelLED) ledArray1 = 0b000000001; // last graph led
  if (alarmLED) {
    ledArray1 = 0b11111111;
    ledArray2 = 0b00001111;
  }

  if (shiftPleaseTimer >= 100) {
    shiftPleaseTimer = 0;
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, (barGraph2[fuelLevel] | ledArray2));
    shiftOut(dataPin, clockPin, MSBFIRST, (barGraph1[fuelLevel] ^ ledArray1));
    digitalWrite(latchPin, HIGH);
    //Serial.println(fuelLevel);
  }
  ledArray2 = 0b00000000;
  ledArray1 = 0b00000000;
}



//  for (int i = 0; i < 10; i++) {
//    digitalWrite(latchPin, LOW);
//
//    shiftOut(dataPin, clockPin, MSBFIRST, (barGraph2[i] | ledArray));
//    //shiftOut(dataPin, clockPin, MSBFIRST, 255);
//    shiftOut(dataPin, clockPin, MSBFIRST, barGraph1[i]);
//
//    digitalWrite(latchPin, HIGH);
//    delay(200);
//  }
