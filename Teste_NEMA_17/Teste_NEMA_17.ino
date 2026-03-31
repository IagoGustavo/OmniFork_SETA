void setup() {
    Serial.begin(9600);
    DDRD |= (1 << PD7);
    DDRB |= (1 << PB0);
}

void loop() {  
  PORTB |= (1 << PB0); // direção

  for(int x = 0; x < 1600; x++) {
    PORTD |= (1 << PD2);
    delayMicroseconds(150);
    PORTD &= ~(1 << PD2);
    delayMicroseconds(150); 
  }
}