#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
int trig = 2;
int echo = 3;

const int GSR = A0;
int sensorValue = 0;
int gsr_average = 0;


void testdrawstyles(int a){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("level="));
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print(a);
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print(F("%"));
  display.display();
  delay(2000);
  }
  
  
  
  long waterLoss() {
  // put your main code here, to run repeatedly:
  long sum = 0;
  for(int i = 0;i < 10;i++){
    sensorValue = analogRead(GSR);
    sum = sum + sensorValue;
    delay(5);
  }
  gsr_average = sum/10;
//   Serial.print("GSR Average is : ");
//   Serial.println(gsr_average);
  int human_resistance = ((1024+2*gsr_average)*10000)/(516 - gsr_average);
//   Serial.print("Human Resistance is : ");
//   Serial.println(human_resistance);
  int skin_conductance = 1000000/human_resistance;
  int sweat_rate = 4.3*skin_conductance - 3.5;
//   Serial.print("Sweat rate is : ");
//   Serial.println(sweat_rate);  
return sweat_rate;
}

void setup() {
 Serial.begin(9600);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT); 

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }



  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(2000);

  //testdrawstyles();    // Draw 'stylized' characters
}

void loop() {
  long t = 0, h = 0, hp = 0;
  
  // Transmitting pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Waiting for pulse
  t = pulseIn(echo, HIGH);
  
  // Calculating distance 
  h = t / 58;
 
  h = h - 6 ;  // offset correction
  h = 20 - h;  // water height, 0 - 50 cm
  //optional 
  //hp = 5 * h;  // distance in %, 0-100 %
  
  // Sending to computer
  Serial.print(hp);
  // Serial.print(" cm\n");
  Serial.print("\n");
  
  delay(1000);
  testdrawstyles(hp); 
}
