#include <Arduino_BuiltIn.h>

#define BLYNK_TEMPLATE_ID "TMPL3efILnn4K"
#define BLYNK_TEMPLATE_NAME "aquasense"
#define BLYNK_AUTH_TOKEN "vUikO5bPMExaxWrShI-3CmhQPy-jpbg4"

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

#define BLYNK_PRINT Serial

const int pingPin = 4; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 5; // Echo Pin of Ultrasonic Sensor

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "sumi";
char pass[] = "sumisona";

#define EspSerial Serial

#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

BlynkTimer timer;

WidgetRTC rtc;
const byte GSR = A0;

int swr = 0;
int flag = 0;
int twl = 0;
int sw = 7;
int twlh = 0, twi = 0;
int hl = 0;
int awater = 0, twater = 0, wat = 0;


// Digital clock display of the time
void clockDisplay()
{
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();
  if (minute() % 2 == 0) {
    wat = readPing();
    if (awater > wat) {
      twi = twi + (awater - wat);
      //  twi = twi + readPing();
    }
    awater = readPing();
  }
  // Send time to the App
  Blynk.virtualWrite(V6, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V7, currentDate);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water= ");
  lcd.print(awater);
  lcd.print(" ml");
  lcd.setCursor(0, 1);
  lcd.print("TWI= ");
  lcd.print(twi);
  lcd.print(" ml");
  if (hour() == 23 && minute() == 59) {
    swr = 0;
    twl = 0;
    twlh = 0;
    twi = 0;
    hl = 0;
    awater = 0;
    twater = 0;
  }
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}
int hp = 0, hlev = 0;
void setup()
{
  // Debug console
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);  //Set cursor to character 2 on line 0
  lcd.print("Connecting To ");
  lcd.setCursor(0, 1);  //Move cursor to character 2 on line 1
  lcd.print(" Internet.... ");
  pinMode(sw, INPUT_PULLUP);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);
  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

  // Display digital clock every 10 seconds
  timer.setInterval(10000L, clockDisplay);

  awater = readPing();
}

void loop()
{
  Blynk.run();
  timer.run();

  if (digitalRead(sw) == LOW)
  {
    lcd.clear();
    lcd.setCursor(0, 0);  //Set cursor to character 2 on line 0
    lcd.print("Taking GSR ");
    lcd.setCursor(0, 1);  //Move cursor to character 2 on line 1
    lcd.print(" Value.... ");
    gsr_read();
    twlh = swr * 2;
    twl = twl + twlh;
    twl=twl/10;
    hl = twi - twl;
    hp = (hl / 1.28) * 10;
    hlev = hp/10;
  }
  Blynk.virtualWrite(V0, twl);
  Blynk.virtualWrite(V1, hlev);
  Blynk.virtualWrite(V2, swr);
  Blynk.virtualWrite(V3, awater);
  Blynk.virtualWrite(V4, twlh);
  Blynk.virtualWrite(V5, twi);

}

int readPing() {
  long duration, cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = duration / 29 / 2;
  if (cm > 20) {
    cm = 0;
  }
  cm = 20 - cm;
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  return cm;
}

void gsr_read()
{
  int sensorValue = 0;
  int gsr_average = 0;
  for (int i = 0; i < 100; i++) {
    long sum = 0;
    for (int i = 0; i < 10; i++)    
    {
      sensorValue = analogRead(GSR);
      sum += sensorValue;
      delay(5);
    }
    gsr_average = sum / 10;
    int Human_Resistance =  ((1024 + 2 * gsr_average) * 10000) / (512 - gsr_average);
    Serial.println(Human_Resistance);
    Blynk.virtualWrite(V8, Human_Resistance);
    if (gsr_average < 400) {
      if (Human_Resistance < 0) {
        flag = flag + 1;
      }
    }
  }
  flag = flag / 10;
  swr =  flag;
  swr = 4.3 * swr - 3.5;
  Serial.println(swr);
}