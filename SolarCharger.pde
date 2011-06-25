/*

  SolarCharger
  
  François Deppierraz <francois@ctrlaltdel.ch>
  
*/

int LEDpin = 13;
int LEDprev = LOW;
int HZ = 1;

float R1 = 98400.0;
float R2 = 46200.0;
float H = (R2/(R1+R2));

char voltagePins[] = {A1, A2, A3, A4, A5};
int countVoltagePins = 5;

int LCDcols = 20;
int LCDlines = 4;

#define BUFSIZE 20

/* LCD device */
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* This function was copied from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1164927646 */
char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}

float readVoltage(char pin) {
  int value = 0;
  float Vout = 0.0;
  float Vin = 0.0;
  
  Serial.print("blah");
  
  //int pin = voltagePins[index];
  value = analogRead(pin);

  Vout = (value * 5.0) / 1024.0;
  Vin = Vout / H;

  return Vin;  
}

void blinkLED(void) {
  if (LEDprev == LOW) {
    LEDprev = HIGH;
  } else {
    LEDprev = LOW;
  }
  digitalWrite(LEDpin, LEDprev);
}

void printVoltages(){
  char buffer[BUFSIZE];

  Serial.print(millis());
  Serial.print(",");
  
  lcd.setCursor(0, 1);
      
  for (int i; i < countVoltagePins; i++) {
    Serial.print(i);
    ftoa(buffer, readVoltage(voltagePins[i]), 1);
    lcd.print(buffer);
    lcd.print(" ");
    
    Serial.print(buffer);
    Serial.print(",");
  }
  
  Serial.println();
}

void displayUptime(void){
  lcd.setCursor(0,3);
  lcd.print(millis()/1000);
  lcd.print(" sec"); 
}

void setup(){
  // Voltage sensors

  for (int i; i < countVoltagePins; i++) {
    pinMode(voltagePins[i], INPUT);
  }
  
  // Heartbeat LED
  pinMode(LEDpin, OUTPUT);
  
  // Serial port is used for debug
  Serial.begin(9600);
  
  // LCD
  lcd.begin(LCDcols, LCDlines);
  
  // Welcome
  lcd.setCursor(0, 0);
  lcd.print("Paleo 2011  Bamboule");
}

void loop(){
  blinkLED();
  printVoltages();
  displayUptime();
  delay(1000/HZ);
}
