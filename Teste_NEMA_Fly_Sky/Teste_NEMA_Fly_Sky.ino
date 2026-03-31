#include <IBusBM.h>

IBusBM ibus;

#define STEP_PIN 7
#define DIR_PIN 8

int canal;

void setup() {
  Serial.begin(115200);
  ibus.begin(Serial);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
}

void loop() {
  canal = ibus.readChannel(2);

  int velocidade = map(canal, 1000, 2000, -1000, 1000);

  if(abs(velocidade) < 50){
    return;
  }

  if(velocidade > 0){
    digitalWrite(DIR_PIN, HIGH);
  }
  else{
    digitalWrite(DIR_PIN, LOW);
    velocidade = -velocidade;
  }

  int delayStep = map(velocidade, 0, 1000, 2000, 200);

  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(delayStep);

  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(delayStep);
}
