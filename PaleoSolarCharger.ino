/*

  PaleoSolarCharger
  
  Paleo 2011-2012
  
  Copyleft François Deppierraz <francois@ctrlaltdel.ch>
  GPL License apply
  
*/

int LEDpin = 13;
int LEDprev = LOW;

int HZ = 1;

float BAT_HIGH = 14.4; // Stop charging when battery reach this voltage
float BAT_THRESHOLD = 13.8; // Restart charging when battery reach this voltage
float BAT_ALARM_LOW = 5.0; // When a battery voltage decrease under this value, raise an alert and stop everything
float BAT_ALARM_HIGH = 15.0;

//char voltagePins[] = { A0, A1, A2, A3, A4 };
char voltagePins[] = { A4, A3, A2 };
int countVoltagePins = 3;

int ALARM_PIN = 8;

int BAT1_ID = 0;
int BAT2_ID = 1;
int BAT3_ID = 2;

int PANEL1_ID = 0;
int PANEL2_ID = 1;
int DCDC1_ID = 2;

/*
float R1[] = {98300.0, 98600.0, 98400.0, 98400.0, 98400.0};
float R2[] = {9910.0, 9930.0, 9940.0, 9910.0, 9920.0};
*/

float H = (9910.0/(98300.0 + 9910.0));
float correction[] = { 12.36/11.9, 25.3/25.0, 38.3/37.6 };

//char relays[] = { 8, 9, 10, 11, 12 };
char relays[] = { 12, 11, 10 };
int relaysState[] = { LOW, LOW, LOW };
int countRelays = 3;

int LCDcols = 20;
int LCDlines = 4;

#define BUFSIZE 20

/* LCD device */
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/* Watchdog */
#include <avr/wdt.h>

float getVoltage(int id) {
  float voltage = readVoltage(id);
 
  // Substract voltages from previous batteries (if any) in the serial bank
  
  if (id > 0) {
    return voltage - readVoltage(id-1);
  } else {
    return voltage;
  }
}

float readVoltage(int index) {
  int value = 0;
  float Vout = 0.0;
  float Vin = 0.0;
  // float H = (R2[index]/(R1[index]+R2[index]));   global
  
  char pin = voltagePins[index];
  value = analogRead(pin);

  //Vout = 0.967 * (value * 5.0) / 1024.0;
  Vout = (value * 5.0) / 1024.0;
  Vin = Vout / H;

  return correction[index] * Vin;  
}

void blinkLED(void) {
  if (LEDprev == LOW) {
    LEDprev = HIGH;
  } else {
    LEDprev = LOW;
  }
  digitalWrite(LEDpin, LEDprev);
}

void initRelays(void) {
  for (int i=0; i < countRelays; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], LOW);
  }
}

void relayOn(int id) {
  digitalWrite(relays[id], HIGH);
  relaysState[id] = HIGH;
}

void relayOff(int id) {
  digitalWrite(relays[id], LOW);
  relaysState[id] = LOW;
}

int getRelayState(int id) {
  return relaysState[id]; 
}

void displayVoltages(float BAT1, float BAT2, float BAT3){
  char buffer[BUFSIZE];

  lcd.setCursor(0, 1);
  
  lcd.print(BAT1);
  lcd.print(" ");
  lcd.print(BAT2);
  lcd.print(" ");
  lcd.print(BAT3);
  lcd.print(" ");

  /* Serial debug */

  Serial.print(millis());
  Serial.print(",");
  Serial.print(BAT1);
  Serial.print(",");
  Serial.print(BAT2);
  Serial.print(",");
  Serial.print(BAT3);
  Serial.println();
}

void displayUptime(void){
  lcd.setCursor(0,3);
  lcd.print(millis()/1000);
  lcd.print(" sec"); 
}

void displayRelaysState(void) {
  lcd.setCursor(0, 2);
  
  for (int i=0; i < countRelays; i++) {
    lcd.print(getRelayState(i));
    lcd.print(" ");
  }
  
}

void chaser(void) {
  for (int i=0; i < 1; i++) {
    if (getRelayState(i) == HIGH) {
      relayOff(i);
    } else {
      relayOn(i);
    }
  }
}


void checkBatteryCharging(float voltage, int relay_id) {
  char buffer[BUFSIZE];
  
  int relay_status = getRelayState(relay_id);
  
  Serial.print("checkBatteryCharging: ");
  Serial.print(voltage);
  Serial.print(" ");
  Serial.print(relay_status);
  Serial.print(" ");
  
  if (voltage <= BAT_ALARM_LOW) {
    die();
  } else if (voltage >= BAT_ALARM_HIGH) {
    die();
  } else if (relay_status == HIGH && voltage >= BAT_HIGH) {
    Serial.println("Relay off");
    relayOff(relay_id);
  } else if (relay_status == LOW) {
    if (voltage < BAT_THRESHOLD) {
      Serial.println("Relay on");
      relayOn(relay_id);
    } else {
      Serial.println("no charging needed");
    }
  } else {
    Serial.println("charging");
  }
}

void controlCharging(float BAT1, float BAT2, float BAT3) {
  checkBatteryCharging(BAT1, PANEL1_ID);
  checkBatteryCharging(BAT2, PANEL2_ID);
  checkBatteryCharging(BAT3, DCDC1_ID);
}

void securityCheck(float BAT1, float BAT2, float BAT3) {
  if (BAT1 > BAT_ALARM_HIGH || BAT2 > BAT_ALARM_HIGH || BAT3 > BAT_ALARM_HIGH) {
    die();
  }
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
  lcd.print("Paleo 2012");
  
  // Relays
  initRelays();
  
  // Init watchdog
  wdt_enable(WDTO_8S);
}

void die() {  
  for (int i=0; i < countRelays; i++)
    relayOff(i);
  
  lcd.setCursor(0, 0);
  lcd.print("Casse...");

  digitalWrite(ALARM_PIN, HIGH);
  
  wdt_disable();
  exit(1);
}
  
void loop(){
  wdt_reset();
  
  blinkLED();
  
  // Get battery voltage
  float BAT1 = getVoltage(BAT1_ID);
  float BAT2 = getVoltage(BAT2_ID);
  float BAT3 = getVoltage(BAT3_ID);

  displayVoltages(BAT1, BAT2, BAT3);
  displayRelaysState();
  displayUptime();

  securityCheck(BAT1, BAT2, BAT3);
  
  controlCharging(BAT1, BAT2, BAT3);
  
  delay(1000/HZ);
}



