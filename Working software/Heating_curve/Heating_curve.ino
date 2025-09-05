#include <PID_v1.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==== OLED Screen SETUP ====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ==== THERMISTOR SETUP ====
const int thermistorPin = A0; 
const double seriesResistor = 10000.0;    // 10kΩ resistor
const double nominalResistance = 10000.0; // 10kΩ at 25°C
const double nominalTemp = 25.0; 
const double bCoefficient = 3950.0;

// ==== HEATER SETUP ====
const int heaterPin = 10; // PWM pin

// ==== PID VARIABLES ====
double Setpoint = 25.0;  // Initial target temp
double Input, Output;
double Kp = 12.0, Ki = 0.8, Kd = 100;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// ==== SERIAL INPUT ====
String inputString = "";

// ==== TIME TRACKING ====
unsigned long startTime = 0;

void setup() {
  Serial.begin(9600);
  startTime = millis(); // record start time

  pinMode(heaterPin, OUTPUT);
  analogWrite(heaterPin, 0);

  myPID.SetOutputLimits(0, 255);
  myPID.SetMode(AUTOMATIC);

  // CSV header
  Serial.println("Time(s),Temperature(C),Setpoint(C)");

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
}

void loop() {
  static unsigned long lastDisplayUpdate = 0;
  unsigned long now = millis();

  // === Read thermistor & compute PID ===
  Input = readThermistorTemp(thermistorPin);
  myPID.Compute();
  analogWrite(heaterPin, (int)Output);

  // === CSV logging (every loop) ===
  float timeSec = (now - startTime) / 1000.0;
  Serial.print(timeSec, 2);
  Serial.print(",");
  Serial.print(Input);
  Serial.print(",");
  Serial.println(Setpoint);   // <---- Added Setpoint in CSV too

  // === Slower OLED update (every 500 ms) ===
  if (now - lastDisplayUpdate >= 500) {
    lastDisplayUpdate = now;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("Beaker Heater"); 
    display.setCursor(0,10);
    display.print(Input); display.print("/"); display.print(Setpoint);
    display.display();
  }

  // Handle incoming serial commands
  handleSerialInput();

  delay(100); // 20 Hz sample rate
}

// === Thermistor reading ===
double readThermistorTemp(int pin) {
  int analogValue = analogRead(pin);
  double voltage = analogValue / 1023.0;
  double resistance = seriesResistor * ((1.0 / voltage) - 1.0);

  double steinhart;
  steinhart = resistance / nominalResistance;    
  steinhart = log(steinhart);                     
  steinhart /= bCoefficient;                     
  steinhart += 1.0 / (nominalTemp + 273.15);     
  steinhart = 1.0 / steinhart;                   
  steinhart -= 273.15;                          

  return steinhart;
}

// === Serial input parser ===
void handleSerialInput() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        parseCommand(inputString);
        inputString = "";
      }
    } else {
      inputString += inChar;
    }
  }
}

void parseCommand(String cmd) {
  cmd.trim();
  if (cmd.startsWith("S:")) {
    Setpoint = cmd.substring(2).toFloat();
    Serial.print("# Setpoint updated to: ");
    Serial.println(Setpoint);   // confirmation back to Python
  } else {
    Serial.println("# Invalid command. Use S:xx");
  }
}
