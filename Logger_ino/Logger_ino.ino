char pins[] = { A0, A1, A2, A3, A4, A5, A6, A7 };
int count_pins = 8;

void setup() {
  Serial.begin(9600); 
}

void loop() {
  for (int i=0; i < count_pins; i++) {
    Serial.print("A");
    Serial.print(i);
    Serial.print("=");                       
    Serial.print(analogRead(pins[i]));
    Serial.print(" ");
  }
  
  Serial.println();   

  delay(1000);                     
}
