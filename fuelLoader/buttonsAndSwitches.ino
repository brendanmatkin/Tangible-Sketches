
elapsedMillis bottleLoadDelay;

void readButtonsAndSwitches() {
  
  if (digitalRead(bottleReed) == LOW) {
    if (bottleLoadDelay >= 3500) {
      bottleLoaded = true;
      //currentFuelInBottle = MAX_FUEL;
      bottleLoadDelay = 0;
    }
  } else {
    bottleLoaded = false;
    //bottleArmed = false;
    bottleLoadDelay = 0;
  }

  armButton.update();
  //if (digitalRead(bottleButton) == LOW) {
  if (armButton.fallingEdge()) {
    if (bottleLoaded) {
      bottleArmed = !bottleArmed;
      if (!bottleArmed) {
        currentFuelInBottle = fuelLevel;
      }
    }
  }

  if (digitalRead(loadSwitch) == LOW) {
    loadFuel = true;
  } else {
    loadFuel = false;
  }

  if (digitalRead(unloadSwitch) == LOW) {
    unloadFuel = true;
  } else {
    unloadFuel = false;
  }

  if (digitalRead(pumpSwitch) == LOW) {
    pumpOn = true;
  } else {
    pumpOn = false;
  }
}

