#include <Wire.h>
#include <GyverOLED.h>

// OLED display object
GyverOLED<SSH1106_128x64> display;

// Constants for the display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Switch pins
#define LEFT_SWITCH_PIN 36 // Choose appropriate digital pins
#define RIGHT_SWITCH_PIN 39

// Function to draw the step function
void drawStepFunction(bool leftSwitchState, bool rightSwitchState) {
  display.clear();

  // Determine the level based on the switch states
  int level = 0;
  if (leftSwitchState) {
    level = 1;  // Low Level
  }
  if (rightSwitchState) {
    level = 2;  // High Level
  }
  if (leftSwitchState && rightSwitchState) {
    level = 3;
  }

  // Calculate the Y coordinate for the step function line
  int yCoordinate;
  if(level==0){
    yCoordinate = SCREEN_HEIGHT - 10;
  }
  else if(level==1){
    yCoordinate = SCREEN_HEIGHT - 20;
  }
    else if(level==2){
    yCoordinate = SCREEN_HEIGHT - 20;
  }
    else{
    yCoordinate = SCREEN_HEIGHT - 30;
  }


  // Draw a horizontal line representing the step function
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    display.dot(x, yCoordinate, 1);
  }

  // Add text to the screen
    display.setCursor(0, 0); // Position the cursor at the top-left
    display.print("Left Switch: ");
    display.print(leftSwitchState ? "ON " : "OFF");
    display.setCursor(0, 1); // Move the cursor down a line
    display.print("Right Switch: ");
    display.print(rightSwitchState ? "ON " : "OFF");
    display.setCursor(0, 2);
  display.update();
}

void setup() {
  display.init();
  Serial.begin(115200);

  // Set switch pins as inputs with internal pull-up resistors
  pinMode(LEFT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(RIGHT_SWITCH_PIN, INPUT_PULLUP);
}

void loop() {
  // Read the switch states
  bool leftSwitchState = !digitalRead(LEFT_SWITCH_PIN); // ! because of INPUT_PULLUP
  bool rightSwitchState = !digitalRead(RIGHT_SWITCH_PIN); // ! because of INPUT_PULLUP

  Serial.print("Left: ");
  Serial.print(leftSwitchState);
  Serial.print(", Right: ");
  Serial.println(rightSwitchState);

  // Draw the step function based on the switch states
  drawStepFunction(leftSwitchState, rightSwitchState);

  delay(50); // Debounce delay
}