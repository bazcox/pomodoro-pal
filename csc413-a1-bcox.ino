// RGB LED pins
const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;

// Button pins
const int buttonPin = 2;
const int startButtonPin = 3;

// Variables
int studyTime = 0;           // Study time in minutes
bool timerRunning = false;   // Flag for study timer
bool breakRunning = false;   // Flag for break timer
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long totalTime = 0;
unsigned long breakTime = 0; // Break time in milliseconds

void setup() {
  // Initialize RGB LED pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Initialize button pins with internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(startButtonPin, INPUT_PULLUP);

  // Initialize Serial Monitor (optional for debugging)
  Serial.begin(9600);
}

void loop() {
  if (!timerRunning && !breakRunning) {
    // Allow user to set the study time
    setStudyTime();
  } else if (timerRunning) {
    // Run the study timer and update LED color
    runStudyTimer();
  } else if (breakRunning) {
    // Run the break timer and update LED color
    runBreakTimer();
  }
}

void setStudyTime() {
  // Read button states
  int buttonState = digitalRead(buttonPin);
  int startButtonState = digitalRead(startButtonPin);

  // Wait for button press to increment time
  if (buttonState == LOW) {
    delay(200); // Debounce delay
    // ***Set Time Back To 5min***
    studyTime += 5; // Increase time by 5 minutes
    if (studyTime > 60) {
      studyTime = 5; // Reset to 5 minutes if over 60
    }
    Serial.print("Study Time Set To: ");
    Serial.print(studyTime);
    Serial.println(" minutes");

    // Provide visual feedback (e.g., flash LED)
    flashColor(0, 255, 255); // Cyan flash
  }

  // Use second button to start the timer
  if (startButtonState == LOW) {
    if (studyTime > 0) { // Ensure study time has been set
      timerRunning = true;
      startTime = millis();
      totalTime = studyTime * 60UL * 1000UL; // Total study time in milliseconds
      Serial.println("Study Timer Started");
    } else {
      Serial.println("Please set study time first!");
    }
    delay(200); // Debounce delay
  }
}

void runStudyTimer() {
  // Calculate elapsed time in milliseconds
  elapsedTime = millis() - startTime;

  // Calculate remaining time
  unsigned long remainingTime = totalTime - elapsedTime;

  // Prevent negative time
  if (remainingTime > totalTime) {
    remainingTime = 0;
  }

  // Calculate percentage of time remaining
  float percentRemaining = (float)remainingTime / totalTime;

  // Update LED color based on remaining time
  updateStudyLEDColor(percentRemaining);

  // Print remaining time to Serial Monitor
  unsigned long remainingSeconds = remainingTime / 1000;
  unsigned int minutes = remainingSeconds / 60;
  unsigned int seconds = remainingSeconds % 60;
  Serial.print("Study Time Left: ");
  Serial.print(minutes);
  Serial.print("m ");
  Serial.print(seconds);
  Serial.println("s");

  // Check if time is up
  if (remainingTime <= 0) {
    timerRunning = false;
    studyTime = 0; // Reset study time
    Serial.println("Study Time's Up!");
    // Signal end of timer
    endTimerSignal();

    // Calculate break time
    calculateBreakTime();

    // Start break timer
    breakRunning = true;
    startTime = millis(); // Reset start time for break
    Serial.print("Break Time: ");
    Serial.print(breakTime / 60000); // Convert milliseconds to minutes
    Serial.println(" minutes");
  }

  delay(1000); // Update every second
}

void runBreakTimer() {
  // Calculate elapsed time in milliseconds
  elapsedTime = millis() - startTime;

  // Calculate remaining break time
  unsigned long remainingTime = breakTime - elapsedTime;

  // Prevent negative time
  if (remainingTime > breakTime) {
    remainingTime = 0;
  }

  // Calculate percentage of break time elapsed
  float percentElapsed = (float)elapsedTime / breakTime;

  // Update LED color based on break time elapsed
  updateBreakLEDColor(percentElapsed);

  // Print remaining break time to Serial Monitor
  unsigned long remainingSeconds = remainingTime / 1000;
  unsigned int minutes = remainingSeconds / 60;
  unsigned int seconds = remainingSeconds % 60;
  Serial.print("Break Time Left: ");
  Serial.print(minutes);
  Serial.print("m ");
  Serial.print(seconds);
  Serial.println("s");

  // Check if break time is up
  if (remainingTime <= 0) {
    breakRunning = false;
    Serial.println("Break Time's Up!");
    // Signal end of break
    endBreakSignal();

    // Reset the device
    resetDevice();
  }

  delay(1000); // Update every second
}

void calculateBreakTime() {
  // For every 5 minutes of study time, add 1 minute of break
  unsigned int breakMinutes = (totalTime / (5 * 60UL * 1000UL));
  breakTime = breakMinutes * 60UL * 1000UL; // Break time in milliseconds
  if (breakTime == 0) {
    breakTime = 1 * 60UL * 1000UL; // Minimum 1 minute break
  }
}

void updateStudyLEDColor(float percentRemaining) {
  // Gradually change color from green to red
  int redValue = (int)((1 - percentRemaining) * 255);
  int greenValue = (int)(percentRemaining * 255);
  int blueValue = 0; // No blue component during study time

  setColor(redValue, greenValue, blueValue);
}

void updateBreakLEDColor(float percentElapsed) {
  // Fade from dark blue to light blue
  // Dark Blue RGB: (0, 0, 50)
  // Light Blue RGB: (150, 200, 255)

  int redValue = (int)(percentElapsed * 150);      // From 0 to 150
  int greenValue = (int)(percentElapsed * 200);    // From 0 to 200
  int blueValue = (int)(50 + percentElapsed * 205); // From 50 to 255

  setColor(redValue, greenValue, blueValue);
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void flashColor(int redValue, int greenValue, int blueValue) {
  setColor(redValue, greenValue, blueValue);
  delay(200);
  setColor(0, 0, 0); // Turn off
  delay(200);
  setColor(redValue, greenValue, blueValue);
  delay(200);
  setColor(0, 0, 0); // Turn off
}

void endTimerSignal() {
  // Flash red LED
  for (int i = 0; i < 3; i++) {
    setColor(255, 0, 0); // Red
    delay(500);
    setColor(0, 0, 0); // Off
    delay(500);
  }
}

void endBreakSignal() {
  // Flash blue LED
  for (int i = 0; i < 3; i++) {
    setColor(0, 0, 255); // Blue
    delay(500);
    setColor(0, 0, 0); // Off
    delay(500);
  }
}

void resetDevice() {
  // Turn off LEDs
  setColor(0, 0, 0);

  // Reset variables
  studyTime = 0;
  elapsedTime = 0;
  totalTime = 0;
  breakTime = 0;

  Serial.println("Device Reset. Ready for new session.");
}
