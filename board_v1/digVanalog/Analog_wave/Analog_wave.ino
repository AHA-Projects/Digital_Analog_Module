#include <Wire.h>
#include <GyverOLED.h>
#include <math.h>

// OLED display object
GyverOLED<SSH1106_128x64> display;

// Constants for the display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define POT_PIN 27

#define ADC_MAX 4095.0

void setup() {
  display.init();
  Serial.begin(115200);
  pinMode(POT_PIN, INPUT);
}

// Function to read the potentiometer value and map it to a desired range (using float arithmetic)
float readPotentiometer(float minVal, float maxVal) {
  float sensorValue = analogRead(POT_PIN);
  float mappedValue = minVal + ((4095 - sensorValue) / ADC_MAX) * (maxVal - minVal);  // Correct mapping for ADC_MAX
  return mappedValue;
}

// Function to draw a sinusoidal wave on the OLED
void drawSineWave(float amplitude, float frequency) {
  display.clear();
  display.setCursor(0, 0); // Position the cursor at the top-left
  display.print("Analog Signal");
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    // Calculate the y-value for the sine wave.  Center it at SCREEN_HEIGHT / 2.
    float yValue = amplitude * sin(2.0 * PI * frequency * (float)x / SCREEN_WIDTH) + SCREEN_HEIGHT / 2.0;

    // Check if the y-value is within the screen boundaries
    if (yValue >= 0 && yValue < SCREEN_HEIGHT) {
      display.dot(x, (int)round(yValue), 1);
    }
  }
  display.update();
}

void loop() {

  // Read amplitude from the potentiometer, mapped to a *smaller* range
  float amplitude = readPotentiometer(5, SCREEN_HEIGHT / 4.0);  // Reduced amplitude range!
  // Read frequency from the potentiometer, mapped to a range
  float frequency = readPotentiometer(0.05, 1.0);

  Serial.print("Amplitude: ");
  Serial.print(amplitude);
  Serial.print(", Frequency: ");
  Serial.println(frequency);


  // Draw the sine wave with the current amplitude and frequency
  drawSineWave(amplitude, frequency);

  // Delay to allow for smooth display updates
  delay(10);
}