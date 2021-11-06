#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DISP595_BRIGHTNESS
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

void loop() {
  checkHumidity();
  fanAdjust();
  disp.tick();
}

void checkHumidity() {
  if(millis() - lastCheck >= INTERVAL) {
    float humidity = dht.readHumidity();
    disp.displayFloatAuto(humidity);
    if(!vaporState && humidity < 37) vaporState = HIGH;
    else if(vaporState && humidity > 43) vaporState = LOW;
    if(lastVaporState != vaporState) digitalWrite(VAPORPIN, vaporState);
    lastVaporState = vaporState;
    lastCheck = millis();
  }
}

void fanAdjust() {
  if(vaporState) {
    if(!lastFanState) lastFanState = HIGH;
    int speed = analogRead(POTPIN);
    speed = map(speed, 0, 1023, 200, 1023);
    Timer1.pwm(FANSPIN, speed);
    digitalWrite(LEDPIN, HIGH); 
  }
  else if(lastFanState) {
    Timer1.pwm(FANSPIN, 50);
    digitalWrite(LEDPIN, LOW);
    lastFanState = LOW;
  }
}
