
#include <Wire.h> //I2C communication
#include <Adafruit_INA260.h>
#include <HX711.h>
#include <Servo.h>

// Define ensors
Adafruit_INA260 ina;
HX711 loadcell;

// ESC
Servo esc;

// Variables
float calFactor = 1.0;
float pct = 0;
long tareRaw = 0;

// throttle function
void setThrottle(float p) { 
  pct = constrain(p, 0, 100);

  int pulseWidth = 1000 + 10 * pct; //1000 off, 2000 = max throttle 

  esc.writeMicroseconds(pulseWidth);
}

void setup() {
  // Initialise ESC
  esc.attach(9);
  setThrottle(0);

  // serial communication
  Serial.begin(115200);
  Serial.setTimeout(100);

  // Start sensors
  ina.begin();
  loadcell.begin(2, 3);

  delay(3000);

  Serial.println("t_ms,throttle_pct,bus_V,current_A,power_W,load_raw,thrust_gf");
}

void loop() {

  // Read serial commands
  if (Serial.available()) {
    char c = Serial.read();

    if (c == 'p') setThrottle(Serial.parseFloat());

    if (c == 't') tareRaw = loadcell.read_average(10);

    if (c == 'k') {
      float g = Serial.parseFloat();
      if (g > 0) {
        calFactor = (loadcell.read_average(10) - tareRaw) / g;
      }
    }
  }

  // Read load cell
  long raw = loadcell.read();

  // Output measurements
  Serial.print(millis());
  Serial.print(',');
  Serial.print(pct, 1);
  Serial.print(',');
  Serial.print(ina.readBusVoltage() / 1000, 3);
  Serial.print(',');
  Serial.print(ina.readCurrent() / 1000, 3);
  Serial.print(',');
  Serial.print(ina.readPower() / 1000, 3);
  Serial.print(',');
  Serial.print(raw);
  Serial.print(',');
  Serial.println((raw - tareRaw) / calFactor, 2);
}
