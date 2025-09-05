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
const int thermistorPins[2] = {A0, A2}; 
const double seriesResistor = 10000.0;   // 10kΩ resistor
const double nominalResistance = 10000.0; // 10kΩ at 25°C
const double nominalTemp = 25.0; 
const double bCoefficient = 3950.0;

// ==== HEATER SETUP ====
const int heaterPins[2] = {10, 9}; // PWM pins

// ==== PID VARIABLES ====
double Setpoints[2] = {25.0, 25.0}; // Initial target temps
double Inputs[2], Outputs[2];
double Kp = 3.0, Ki = 6.0, Kd = 4.0;

PID myPIDs[2] = {
  PID(&Inputs[0], &Outputs[0], &Setpoints[0], Kp, Ki, Kd, DIRECT),
  PID(&Inputs[1], &Outputs[1], &Setpoints[1], Kp, Ki, Kd, DIRECT)
};

// ==== SERIAL INPUT ====
String inputString = "";

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) {
    pinMode(heaterPins[i], OUTPUT);
    analogWrite(heaterPins[i], 0);
    myPIDs[i].SetOutputLimits(0, 255);
    myPIDs[i].SetMode(AUTOMATIC);
  }

  Serial.println("Dual PID Heater Control Started");
  Serial.println("Enter setpoints as: S1:50 or S2:40");

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
}

void loop() {
  // === Read thermistors & compute PID ===
  for (int i = 0; i < 2; i++) {
    Inputs[i] = readThermistorTemp(thermistorPins[i]);
    myPIDs[i].Compute();
    analogWrite(heaterPins[i], (int)Outputs[i]);
  }

  // === Display on Serial ===
  Serial.print("Beaker heater => SP: ");
  Serial.print(Setpoints[0]);
  Serial.print(" | Temp: ");
  Serial.print(Inputs[0]);
  Serial.print(" | Out: ");
  Serial.println(Outputs[0]);

  Serial.print("Gas Heater => SP: ");
  Serial.print(Setpoints[1]);
  Serial.print(" | Temp: ");
  Serial.print(Inputs[1]);
  Serial.print(" | Out: ");
  Serial.println(Outputs[1]);

  // === Handle serial input ===
  handleSerialInput();

  // === OLED display ===
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Sensor Heater: "); 
  display.setCursor(0,10);
  display.print(Inputs[0]); display.print("/"); display.print(Setpoints[0]);
  display.setCursor(0,30);
  display.print("Gas Heater: "); 
  display.setCursor(0,40);
  display.print(Inputs[1]); display.print("/"); display.print(Setpoints[1]);
  display.display();

  delay(500);
}

// === Thermistor function ===
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

// === Parse serial input ===
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
  if (cmd.startsWith("S1:")) {
    Setpoints[0] = cmd.substring(3).toFloat();
    Serial.print("Setpoint1 updated to: ");
    Serial.println(Setpoints[0]);
  }
  else if (cmd.startsWith("S2:")) {
    Setpoints[1] = cmd.substring(3).toFloat();
    Serial.print("Setpoint2 updated to: ");
    Serial.println(Setpoints[1]);
  }
  else {
    Serial.println("Invalid command. Use S1:xx or S2:xx");
  }
}
