// Acknowledgments
// Creator: Anany Sharma at the University of Florida working under NSF grant. 2405373
// This material is based upon work supported by the National Science Foundation under Grant No. 2405373.
// Any opinions, findings, and conclusions or recommendations expressed in this material are those of the authors and do not necessarily reflect the views of the National Science Foundation.

#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Required for SPI communication
#include <math.h>            // Needed for plotting the sin graph

// --- Display Pin Definitions ---
#define TFT_CS   33  // Chip Select control pin
#define TFT_DC   25  // Data/Command select pin
#define TFT_RST  26  // Reset pin (or connect to RST, see below)

// Initialize ST7789 TFT library
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// --- Display Constants (Adjusted for 320x170 landscape) ---
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 170

// Initialize GFX Canvas for Double Buffering
GFXcanvas16 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);

// Input Pins (Assuming the same pins for ESP32)
#define POT_PIN 32       // Analog input for potentiometer
#define LEFT_SWITCH_PIN 36 // Digital input for left switch
#define RIGHT_SWITCH_PIN 39 // Digital input for right switch

#define ADC_MAX 4095.0

// --- Global Variable ---
// Define the Y coordinate for the base digital level (both switches OFF)
// Let's place it about 30 pixels from the bottom for better visibility on a taller screen.
const int BASE_Y_LEVEL = SCREEN_HEIGHT - 30; // Used as centerY for sine wave

// --- Colors (Using 16-bit color values) ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define BLUE    0x001F
#define GREEN   0x07E0
#define RED     0xF800

void setup() {
  Serial.begin(115200);

  // --- Display Initialization ---
  tft.init(SCREEN_HEIGHT, SCREEN_WIDTH); // Initialize ST7789 with your resolution
  tft.setRotation(3);
  tft.fillScreen(BLACK); // Clear the physical screen once initially

  // Initialize inputs
  pinMode(POT_PIN, INPUT);
  pinMode(LEFT_SWITCH_PIN, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(RIGHT_SWITCH_PIN, INPUT_PULLUP); // Use internal pull-up resistor

  // Draw initial message to canvas, then to TFT
  canvas.fillScreen(BLACK); // Clear canvas
  canvas.setCursor(0, 0);       // Set cursor to top-left
  canvas.setTextColor(WHITE); // Set text color to white
  canvas.setTextSize(2);        // Set text size
  canvas.println("Analog/Digital Demo");
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT); // Display on TFT

  delay(1000); // Display initial message for a moment
}

// Function to read the potentiometer value and map it to a desired range
float readPotentiometer(float minVal, float maxVal) {
  float sensorValue = analogRead(POT_PIN);
  float mappedValue = minVal + (sensorValue / ADC_MAX) * (maxVal - minVal);
  return mappedValue;
}

// Function to draw a sinusoidal wave centered at a specific Y coordinate onto a GFX object
// Amplitude here is the peak deviation from the center Y
void drawSineWave(Adafruit_GFX &gfx, float amplitude, float frequency, int centerY) {
  int gfx_width = gfx.width();
  int gfx_height = gfx.height();
  int maxY = gfx_height - 1;
  int minY_analog_area = 40; // Leave space for text at the top

  if (centerY + amplitude >= maxY) amplitude = maxY - 1 - centerY;
  if (centerY - amplitude < minY_analog_area) amplitude = centerY - minY_analog_area;

  if (amplitude < 1) amplitude = 1;
  for (int x = 0; x < gfx_width; x++) {
    // Calculate the y-value for the sine wave relative to the center Y
    float yValue = amplitude * sin(2.0 * PI * frequency * (float)x / gfx_width) + centerY;

    // Draw the point if it's within screen bounds and the analog area
    if (yValue >= minY_analog_area && yValue < gfx_height) {
      gfx.drawPixel(x, (int)round(yValue), WHITE); // Draw the pixel
    }
  }
}

// Function to draw the digital step function line and associated text onto a GFX object
void drawDigitalStep(Adafruit_GFX &gfx, bool leftState, bool rightState) {
  int gfx_width = gfx.width();
  int gfx_height = gfx.height();
  
  int yCoordinate;
  String levelText;

  if (leftState && !rightState) { // Left ON only
    yCoordinate = gfx_height - 30; // Level 1 near the base
    levelText = "0";
  } else if (!leftState && rightState) { // Right ON only
    yCoordinate = gfx_height - 70; // Level 2 higher
    levelText = "1";
  } else if (leftState && rightState) { // Both ON
    yCoordinate = gfx_height - 110; // Level 3 even higher
    levelText = "0&1"; // Or "BOTH", "MAX"
  } else {
    // This case should not be reached if called from the main loop's "else" branch,
    // as it implies both switches are off (which would be analog mode).
    // However, as a fallback:
    yCoordinate = gfx_height - 30;
    levelText = "ERR";
  }

  gfx.drawFastHLine(0, yCoordinate, gfx_width, GREEN);
  
  gfx.setCursor(0, 0);
  gfx.setTextColor(WHITE);
  gfx.setTextSize(2);
  gfx.println("Mode: Digital Step");

  gfx.setCursor(0, 20); // Move down
  gfx.setTextColor(WHITE);
  gfx.print("L:");
  gfx.print(leftState ? "ON " : "OFF");
  gfx.print(" R:");
  gfx.println(rightState ? "ON " : "OFF");

  gfx.setCursor(120, 50); // Position for "LEVEL:" text
  gfx.print("LEVEL: ");
  gfx.setTextColor(GREEN); // Color for the level text
  gfx.println(levelText);
}


void loop() {
  // Read the switch states
  bool leftSwitchState = !digitalRead(LEFT_SWITCH_PIN);
  bool rightSwitchState = !digitalRead(RIGHT_SWITCH_PIN);

  // --- Start all drawing on the canvas ---
  canvas.fillScreen(BLACK); // Clear the canvas for the new frame

  // --- Conditional Logic: Decide whether to show Analog or Digital ---
  if (!leftSwitchState && !rightSwitchState) {
    // --- ANALOG MODE: No switches pressed ---

    // Read amplitude & frequency from potentiometer
    // Max amplitude should ensure sine wave doesn't go into the top text area
    float max_analog_amplitude = BASE_Y_LEVEL - 40; // Max deviation from BASE_Y_LEVEL upwards
    if (max_analog_amplitude < 1) max_analog_amplitude = 1; // Ensure minimum amplitude range
    
    float amplitude = readPotentiometer(1.0, max_analog_amplitude);
    float frequency = readPotentiometer(0.5, 10.0); // Adjusted frequency range

    // Display Status Text for Analog Mode on canvas
    canvas.setCursor(0, 0);
    canvas.setTextColor(WHITE);
    canvas.setTextSize(2);
    canvas.println("Mode: Analog Wave");

    canvas.setCursor(0, 20);
    canvas.print("Freq: ");
    canvas.setTextColor(BLUE);
    canvas.print(frequency, 1);
    canvas.println(" cycles");
    // Note: Amplitude is not displayed here, but could be added similarly

    drawSineWave(canvas, amplitude, frequency, BASE_Y_LEVEL);

  } else {
    // --- DIGITAL MODE: At least one switch is pressed ---
    drawDigitalStep(canvas, leftSwitchState, rightSwitchState);
  }

  // --- All drawing is done on the canvas, now copy the entire canvas to the TFT ---
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  
  delay(1); // Small delay, can be adjusted or removed depending on performance.
            // The drawing and bitmap transfer will determine the actual frame rate.
}