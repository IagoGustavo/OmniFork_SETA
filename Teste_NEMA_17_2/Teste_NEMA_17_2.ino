/* 
  - Chave A: Chave Fechada (pino D8)
  - Chave B: Chave Aberta (pino D9)
*/

volatile uint8_t estado_anterior_port_b;

volatile bool chave_A_acionada = false, chave_B_acionada = false;

void setup() {
    Serial.begin(9600);
    DDRB &= ~((1 << PB0) | (1 << PB1)); // Chave A e Chave B
    PORTB |= (1 << PB0) | (1 << PB1);

    DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4); // STEP, DIR e ENABLE

    PCICR |= (1 << PCIE0); // Habilita interrupção para o grupo de pinos PORTB
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT1); // Habilita interrupção nos pinos D8 e D9

    estado_anterior_port_b = PINB;
    Serial.println(PINB);
}

void loop() {
  if (chave_A_acionada) {
    PORTD &= ~(1 << PD4);
  }

  if (chave_B_acionada) {
    PORTD |= (1 << PD4);
  }

  else {
  
    PORTD |= (1 << PD3); // direção

    for(int x = 0; x < 1600; x++) {
      PORTD |= (1 << PD2);
      delayMicroseconds(150);
      PORTD &= ~(1 << PD2);
      delayMicroseconds(150); 
    }
  }
}

// Rotina de Serviço de Interrupção do PORTB
ISR(PCINT0_vect) {
  uint8_t estado_atual = PINB;

  if (!(estado_anterior_port_b & (1 << 0)) && (estado_atual & (1 << 0))) {
    chave_A_acionada = true;
    Serial.println("Entrada A");
  }

  else if ((estado_anterior_port_b & (1 << 0)) && !(estado_atual & (1 << 0))) {
    chave_A_acionada = false;
    Serial.println("Saida A");
  }

  if ((estado_anterior_port_b & (1 << 1)) && !(estado_atual & (1 << 1))) {
    chave_B_acionada = true;
    Serial.println("Entrada B");
  }

  else if (!(estado_anterior_port_b & (1 << 1)) && (estado_atual & (1 << 1))) {
    chave_B_acionada = false;
    Serial.println("Saida B");
  }

  estado_anterior_port_b = estado_atual;
}