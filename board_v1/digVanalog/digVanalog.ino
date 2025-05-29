#include <GyverOLED.h>

// Define OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED display object
GyverOLED<SSH1106_128x64> display;

const int leftSwitch = 36;
const int rightSwitch = 39;
const int potPin = 27;

// LED pins
const int redLED = 5;
const int greenLED = 19;
const int blueLED = 18;

void setup() {
  display.init();
  // Initialize serial communication
  Serial.begin(9600);

  // Set switch pins as inputs with internal pull-up resistors
  pinMode(leftSwitch, INPUT_PULLUP);
  pinMode(rightSwitch, INPUT_PULLUP);
  
  // Set LED pins as outputs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  // Ensure all LEDs start off
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(blueLED, LOW);
}

void loop() {
  // Read switch and potentiometer values
  int leftSwitchState = digitalRead(leftSwitch);
  int rightSwitchState = digitalRead(rightSwitch);
  int potValue = analogRead(potPin);

  // Control LED behavior based on switch states (inverse logic due to pull-up resistors)
  if (leftSwitchState == LOW && rightSwitchState == HIGH) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
  } else if (leftSwitchState == HIGH && rightSwitchState == LOW) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
  } else if (leftSwitchState == LOW && rightSwitchState == LOW) {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
  } else {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
  }

  // Display switch status
  display.clear();
  display.setScale(1);
  display.setCursor(0, 0);
  
  if (leftSwitchState == LOW && rightSwitchState == HIGH) {
    display.print("Left Pressed");
  } else if (leftSwitchState == HIGH && rightSwitchState == LOW) {
    display.print("Right Pressed");
  } else if (leftSwitchState == LOW && rightSwitchState == LOW) {
    display.print("Both Pressed");
  } else {
    display.print("None Pressed");
  }

  // Display analog potentiometer value
  display.setCursor(0, 1); // Set cursor to a new line to avoid overlap
  display.print("Pot Rotation: ");
  display.print(((4095 - potValue) * 100.0) / 4095);
  display.println("%");
  display.update();

  // Print potentiometer value to serial monitor
  Serial.print("Potentiometer value: ");
  Serial.println(potValue);

  delay(100);
}