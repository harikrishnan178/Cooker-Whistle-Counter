# Cooker Whistle Counter

## Overview
The **Automated Whistle Counter** is an Arduino-based system that detects and counts whistles using a microphone and an FFT algorithm. The system tracks the number of whistles, displays real-time data on an OLED screen, and alerts the user when a predefined whistle count is reached. This project is particularly useful for applications like pressure cooker monitoring, sports training, or whistle-based signaling systems.

## Features
- Detects whistle frequencies between **400 Hz and 3000 Hz** using FFT.
- Adjustable whistle count target via **increment (+) and decrement (-) buttons**.
- Displays real-time whistle count and status on a **128x32 SSD1306 OLED display**.
- **Buzzer alert** when the target whistle count is reached.
- **Start/Stop button** to control the cooking or monitoring process.
- **Debounce mechanism** to prevent false triggers from button presses or noise.

## Components Required
- **ESP32 / Arduino Nano / Any compatible microcontroller**
- **Microphone module (e.g., KY-038, MAX9814, or any analog mic)**
- **128x32 OLED Display (SSD1306)**
- **Buzzer**
- **Push Buttons (3)** (Start/Stop, Increment, Decrement)
- **Resistors & Jumper Wires**

## Circuit Diagram
1. **Microphone Module**
   - Output (AO) → **A0** (Analog Input of MCU)
   - VCC → **3.3V / 5V**
   - GND → **GND**
2. **OLED Display (SSD1306 I2C)**
   - SDA → **A4 / D21**
   - SCL → **A5 / D22**
   - VCC → **3.3V / 5V**
   - GND → **GND**
3. **Push Buttons**
   - Start/Stop → **D4**
   - Increment (+) → **D2**
   - Decrement (-) → **D3**
4. **Buzzer**
   - Positive → **D12**
   - Negative → **GND**

## Installation & Setup
1. **Install Required Libraries**:
   - **ArduinoFFT** (`ArduinoFFT.h`)
   - **Adafruit SSD1306** (`Adafruit_SSD1306.h`)
   - **Adafruit GFX** (`Adafruit_GFX.h`)
2. **Upload Code to Microcontroller**:
   - Connect the microcontroller via USB.
   - Select the correct board and port in the Arduino IDE.
   - Compile and upload the code.
3. **Power Up the System**:
   - Ensure all connections are secure.
   - Power the microcontroller using USB or an external power source.

## How It Works
1. **Set the Target Whistle Count**
   - Use the `+` and `-` buttons to set the desired number of whistles.
2. **Start the Counting Process**
   - Press the `Start/Stop` button to begin detection.
   - The system continuously listens for whistle frequencies.
3. **Whistle Detection & Counting**
   - The system captures sound via the microphone.
   - It applies **Fast Fourier Transform (FFT)** to identify peak frequency.
   - If the frequency matches the whistle range, it increments the count.
4. **Trigger Alert When Target is Reached**
   - When the set number of whistles is detected, the buzzer sounds an alert.
   - The OLED screen displays **"Cooking Done!"**
5. **Reset or Stop the System**
   - Press `Start/Stop` to reset for a new session.
   - Press `-` to manually stop the buzzer and reset the count.

## Serial Monitor Debugging
- **Detected Frequency**: Displays real-time detected frequencies.
- **Whistle Count Status**: Shows the number of whistles counted.
- **Target Reached Message**: Alerts when the target whistle count is met.

## Future Improvements
- **Integrate Wi-Fi (ESP32) for remote monitoring**
- **Data logging to SD card or cloud storage**
- **Enhanced noise filtering for better accuracy**

## License
This project is open-source and available under the **MIT License**. Feel free to modify and improve it!

## Author
Developed by **Harikrishnan**

For contributions and discussions, visit the **GitHub Repository** and submit a pull request or issue.

---
Happy Coding! 🚀

