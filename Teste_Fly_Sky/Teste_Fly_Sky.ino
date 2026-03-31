#include <IBusBM.h>

IBusBM ibus;

// Lado esquerdo
#define ENA 10
#define IN1 2
#define IN2 3

// Lado direito
#define ENB 11
#define IN3 4
#define IN4 5

int frente;
int direcao;
int giro_eixo;

int motorEsq;
int motorDir;

void giro_proprio_eixo(int velocidade, int pinIN1, int pinIN2, int pinENA, int pinIN3, int pinIN4, int pinENB) {
  if(velocidade > 0) {
    digitalWrite(pinIN1, HIGH);
    digitalWrite(pinIN2, LOW);
    analogWrite(pinENA, velocidade);

    digitalWrite(pinIN3, HIGH);
    digitalWrite(pinIN4, LOW);
    analogWrite(pinENB, velocidade);
  }

  else if(velocidade < 0) {
    digitalWrite(pinIN1, LOW);
    digitalWrite(pinIN2, HIGH);
    analogWrite(pinENA, -velocidade);

    digitalWrite(pinIN3, LOW);
    digitalWrite(pinIN4, HIGH);
    analogWrite(pinENB, -velocidade);
  }

  else {
    digitalWrite(pinIN1, LOW);
    digitalWrite(pinIN2, LOW);
    analogWrite(pinENA, 0);

    digitalWrite(pinIN3, LOW);
    digitalWrite(pinIN4, LOW);
    analogWrite(pinENB, 0);
  }
}


void controlarMotor(int velocidade, int IN_A, int IN_B, int EN) {
  if(velocidade > 0) {

    digitalWrite(IN_A, HIGH);
    digitalWrite(IN_B, LOW);
    analogWrite(EN, velocidade);

  }

  else if(velocidade < 0) {
    digitalWrite(IN_A, LOW);
    digitalWrite(IN_B, HIGH);
    analogWrite(EN, -velocidade);
  }

  else {
    digitalWrite(IN_A, LOW);
    digitalWrite(IN_B, LOW);
    analogWrite(EN, 0);
  }
}

void setup() {

  Serial.begin(115200);

  ibus.begin(Serial);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  direcao = map(ibus.readChannel(0), 1000, 2000, -255, 255);
  frente = map(ibus.readChannel(1), 1000, 2000, -255, 255);
  giro_eixo = map(ibus.readChannel(3), 1000, 2000, -255, 255);

  if(abs(direcao) < 20) direcao = 0;
  if(abs(frente) < 20) frente = 0;
  if(abs(giro_eixo) < 20) giro_eixo = 0;

  motorEsq = direcao + frente;
  motorDir = direcao - frente;

  motorEsq = constrain(motorEsq, -255, 255);
  motorDir = constrain(motorDir, -255, 255);

  controlarMotor(motorEsq, IN1, IN2, ENA);
  controlarMotor(motorDir, IN3, IN4, ENB);
  giro_proprio_eixo(giro_eixo, IN1, IN2, ENA, IN3, IN4, ENB);

  delay(10);
}