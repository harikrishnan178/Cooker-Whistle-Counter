#include <Wire.h>
#include <arduinoFFT.h>
#include <Adafruit_SSD1306.h>
//#include <Adafruit_GFX.h>

#define MIC_PIN A0  // Microphone Analog Input
#define BUZZER_PIN 12
#define START_STOP_BUTTON 4
#define INC_BUTTON 2
#define DEC_BUTTON 3

#define WHISTLE_MIN_FREQ 1000  // Min whistle frequency (Hz)
#define WHISTLE_MAX_FREQ 5000  // Max whistle frequency (Hz)
#define WHISTLE_DEBOUNCE_TIME 20000 // Time interval (ms) between two valid whistles
#define WHISTLE_DURATION 2000    // Whistle must last at least 3 seconds
#define BUTTON_DEBOUNCE_DELAY 300 // Debounce delay for buttons (ms)

const uint16_t samples = 32; // Optimized for memory
const double samplingFrequency = 10000; // Sampling rate

double vReal[samples];
double vImag[samples];

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

int whistleCount = 0;
int targetWhistles = 0;
bool cookingStarted = false;
unsigned long lastWhistleTime = 0;
unsigned long whistleStartTime = 0;
bool whistleInProgress = false;
unsigned long lastButtonPress = 0; // For debounce

Adafruit_SSD1306 display(128, 32, &Wire, -1);  // Initialize SSD1306 display (Address 0x3C)

void setup() {
  Serial.begin(115200);
  pinMode(MIC_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(START_STOP_BUTTON, INPUT_PULLUP);
  pinMode(INC_BUTTON, INPUT_PULLUP);
  pinMode(DEC_BUTTON, INPUT_PULLUP);

  // Initialize the SSD1306 display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    //Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  
  display.display();
  delay(2000); // Wait for the display to initialize
  resetCounter(); // Ensure initial state is properly set
}

void loop() {
  if (digitalRead(START_STOP_BUTTON) == LOW && millis() - lastButtonPress > BUTTON_DEBOUNCE_DELAY) {
    lastButtonPress = millis();
    toggleCooking();
  }
  if (digitalRead(INC_BUTTON) == LOW && millis() - lastButtonPress > BUTTON_DEBOUNCE_DELAY) {
    lastButtonPress = millis();
    increaseCount();
  }
  if (digitalRead(DEC_BUTTON) == LOW && millis() - lastButtonPress > BUTTON_DEBOUNCE_DELAY) {
    lastButtonPress = millis();
    decreaseCount();
  }

  if (cookingStarted) {
    detectWhistle();
  }
}

void detectWhistle() {
  double peakFrequency = detectFrequency();  // Use optimized frequency detection

  Serial.print(peakFrequency, 2);
  Serial.println(" Hz");
  delay(500);
  bool isValidWhistle = (peakFrequency > WHISTLE_MIN_FREQ && peakFrequency < WHISTLE_MAX_FREQ);

  if (isValidWhistle) {
    if (!whistleInProgress) {
      whistleStartTime = millis();
      whistleInProgress = true;
    } else if (millis() - whistleStartTime >= WHISTLE_DURATION) {
      countWhistle();
      whistleInProgress = false;
    }
  } else {
    whistleInProgress = false;
  }
}

double detectFrequency() {
  unsigned long startMicros = micros();  

  // Read microphone data
  double sum = 0;
  for (uint16_t i = 0; i < samples; i++) {
    vReal[i] = analogRead(MIC_PIN);
    sum += vReal[i];
    vImag[i] = 0.0;
    
  }

  // Remove DC Bias
  double average = sum / samples;
  for (uint16_t i = 0; i < samples; i++) {
    vReal[i] -= average;
  }

  // Perform FFT analysis
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);  // Better for sound signals
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  // Measure the actual sampling rate
  unsigned long endMicros = micros();
  double realSamplingFrequency = samples * 1000000.0 / (endMicros - startMicros);

  // Find peak frequency
  return FFT.majorPeak();
}

void countWhistle() {
  unsigned long currentTime = millis();
  if (currentTime - lastWhistleTime > WHISTLE_DEBOUNCE_TIME) {
    whistleCount++;
    lastWhistleTime = currentTime;
    //Serial.println("Whistle Counted!");
    updateDisplay2();  // Update display with new whistle count
    updateSerialMonitor();
  }

  if (whistleCount >= targetWhistles) {
    //Serial.println("Target Whistle Count Reached!");
    triggerAlert();
  }
}

void updateSerialMonitor() {
  //Serial.println("Whistles: " + String(whistleCount) + "/" + String(targetWhistles));
}

void triggerAlert() {
  digitalWrite(BUZZER_PIN, HIGH);
  //Serial.println("Cooking Done! Press Stop to Restart or Press Dec to Stop");
  updateDisplay3();

  while (true) {
    if (digitalRead(START_STOP_BUTTON) == LOW && millis() - lastButtonPress > BUTTON_DEBOUNCE_DELAY) {
      lastButtonPress = millis();
      digitalWrite(BUZZER_PIN, LOW);
      toggleCooking();  // Restart cooking with initial settings
      return;
    }

    if (digitalRead(DEC_BUTTON) == LOW && millis() - lastButtonPress > BUTTON_DEBOUNCE_DELAY) {
      lastButtonPress = millis();
      digitalWrite(BUZZER_PIN, LOW);
      resetCounter();
      return;
    }
  }
}

void toggleCooking() {
  if (cookingStarted) {
    cookingStarted = false;
    //Serial.println("\n--- Cooking stopped! Returning to Set Menu ---");
    resetCounter();  // Reset everything to the set state
  } else {
    cookingStarted = true;
    whistleCount = 0;
    lastWhistleTime = 0;
    //Serial.println("\n--- Cooking started! ---");
    updateSerialMonitor();
    updateDisplay2();  // Update display when cooking starts
  }
}

void resetCounter() {
  cookingStarted = false;
  whistleCount = 0;
  digitalWrite(BUZZER_PIN, LOW);
  //Serial.println("\n--- Set ---");
  //Serial.println("Set Whistles: " + String(targetWhistles));
  //Serial.println("Press Start/Stop to Start Cooking");
  updateDisplay();  // Update display with initial settings
  delay(300); // Extra debounce to prevent multiple prints
}

void increaseCount() {
  if (targetWhistles < 20) {
    targetWhistles++;
    //Serial.println("Whistle Count Set To: " + String(targetWhistles));
    updateDisplay2();  // Update display when count increases
    delay(BUTTON_DEBOUNCE_DELAY);
  }
}

void decreaseCount() {
  if (targetWhistles > 1) {
    targetWhistles--;
    //Serial.println("Whistle Count Set To: " + String(targetWhistles));
    updateDisplay2();  // Update display when count decreases
    delay(BUTTON_DEBOUNCE_DELAY);
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0, 0);     
  display.print("Set Whistle Count");
  display.println();
  //display.print("Whistles: ");
  //display.print(whistleCount);
  //display.print("/");
  //display.print(targetWhistles);
  display.println();
  //display.print("Status: ");
  //display.print(cookingStarted ? "" : "            Press");
  display.println();
  display.print("  +      -     ");
  display.print(cookingStarted ? "Stop" : "");
  display.display();
}

void updateDisplay2() {
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0, 0);     
  //display.print("Set Whistle Count");
  //display.println();
  display.print("Whistles: ");
  display.print(whistleCount);
  display.print("/");
  display.print(targetWhistles);
  display.println();
  display.print("     ");
  display.print(cookingStarted ? "Processing..." : "       ");
  display.println();
  display.println();
  display.print("  +      -     ");
  display.print(cookingStarted ? "Stop" : "Start");
  display.display();
}

void updateDisplay3() {
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0, 0);     
  //display.print("Set Whistle Count");
  //display.println();
  //display.print("Whistles: ");
  //display.print(whistleCount);
  //display.print("/");
  //display.print(targetWhistles);
  display.println();
  display.print("     ");
  display.print(cookingStarted ? "Cooked    Press" : "       ");
  display.println();
  display.println();
  display.print("               ");
  display.print(cookingStarted ? "Stop" : "Start");
  display.display();
}