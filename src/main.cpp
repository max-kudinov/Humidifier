// Automatic Humidifier program 

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DISP595_BRIGHTNESS  // Needs to be defined before including SevenSegmentsDisp library
#include <SevenSegmentsDisp.h>
#include <TimerOne.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define POTPIN 0
#define FANSPIN 10
#define VAPORPIN 3
#define LEDPIN 8
#define INTERVAL 1000

#define SCLK 7
#define RCLK 6
#define DIO 5

unsigned long lastCheck = 0;
boolean vaporState = HIGH;
boolean lastVaporState = LOW;
boolean lastFanState = HIGH;

DHT dht(DHTPIN, DHTTYPE);
Disp595 disp(DIO, SCLK, RCLK);

void checkHumidity();
void fanAdjust();

void setup() {
  pinMode(VAPORPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  Timer1.initialize(40);
  dht.begin();
  disp.brightness(1);
}

// Main loop

void loop() {
  checkHumidity();  // Read humidity level from the sensor and adjust vaporizer state
  fanAdjust();      // Adjust fan speed from potentiometer
  disp.tick();      // Refresh display indication
}

void checkHumidity() {
  if(millis() - lastCheck >= INTERVAL) {  // Do this in a time interval 
    float humidity = dht.readHumidity();  // Read the mesurements
    disp.displayFloatAuto(humidity);      // Output it to display
    if(!vaporState && humidity < 37) vaporState = HIGH;      // Turn vaporizer on if it's currently off and humidity below 37%
    else if(vaporState && humidity > 43) vaporState = LOW;   // Turn it off if it's on and humidity above 43%
    if(lastVaporState != vaporState) digitalWrite(VAPORPIN, vaporState);  // Update if changed
    lastVaporState = vaporState;  // Remember last state
    lastCheck = millis();  // Reset timer
  }
}

void fanAdjust() {
  if(vaporState) {  // Set fan speed if vaporizer is on
    if(!lastFanState) lastFanState = HIGH;  // If fan was previously off remember that it's on 
    int speed = analogRead(POTPIN);  // Read potentiometer state
    speed = map(speed, 0, 1023, 200, 1023);  // Set minimum value as 200
    Timer1.pwm(FANSPIN, speed);  // Set the speed
    digitalWrite(LEDPIN, HIGH);  // Turn on LED
  }
  else if(lastFanState) {  // If vaporizer is off and fan was on, turn off the fan and LED
    Timer1.pwm(FANSPIN, 50);
    digitalWrite(LEDPIN, LOW);
    lastFanState = LOW;
  }
}
