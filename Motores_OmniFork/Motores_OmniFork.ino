#include <IBusBM.h>

IBusBM ibus;

const uint8_t IN1_A = 16;
const uint8_t IN2_A = 15;
const uint8_t IN3_A = 14;
const uint8_t IN4_A = 13;
const uint8_t ENA_A = 10;
const uint8_t ENB_A = 11;

const uint8_t IN1_B = 2;
const uint8_t IN2_B = 3;
const uint8_t IN3_B = 4;
const uint8_t IN4_B = 5;
const uint8_t ENA_B = 6;
const uint8_t ENB_B = 9;

const uint8_t pino_pulsos = 7;
const uint8_t pino_direcao = 8;

const uint8_t chave_cima = 18;
const uint8_t chave_baixo = 19;

bool limiteCima;
bool limiteBaixo;

typedef enum {
  MOTOR_1,
  MOTOR_2,
  MOTOR_3,
  MOTOR_4,
  MOTOR_NEMA
} motor_id_t;

typedef struct MotorDC {
  motor_id_t id;
  const uint8_t pino_direcao_1;
  const uint8_t pino_direcao_2;
  const uint8_t pino_velocidade;
  int velocidade_mov;
  int velocidade_giro;

  MotorDC(motor_id_t id, const uint8_t pino_direcao_1, const uint8_t pino_direcao_2, const uint8_t pino_velocidade): 
    id(id),
    pino_direcao_1(pino_direcao_1),
    pino_direcao_2(pino_direcao_2),
    pino_velocidade(pino_velocidade),
    velocidade_mov(0),
    velocidade_giro(0) {}
} MotorDC;

MotorDC motor_1(MOTOR_1, IN1_A, IN2_A, ENA_A);
MotorDC motor_2(MOTOR_2, IN3_A, IN4_A, ENB_A);
MotorDC motor_3(MOTOR_3, IN1_B, IN2_B, ENA_B);
MotorDC motor_4(MOTOR_4, IN3_B, IN4_B, ENB_B);

typedef struct MotorDePasso {
  motor_id_t id;
  const uint8_t pino_pulsos;
  const uint8_t pino_direcao;
  int delayStep;

  MotorDePasso(motor_id_t id, const uint8_t pino_pulsos, const uint8_t pino_direcao): 
    id(id),
    pino_pulsos(pino_pulsos),
    pino_direcao(pino_direcao),
    delayStep(0) {}
} MotorDePasso;

MotorDePasso nema_17(MOTOR_NEMA, pino_pulsos, pino_direcao);

typedef struct Velocidade {
  int tras_frente;
  int esquerda_direita;
  int giro_eixo;
  int garfo;

  Velocidade(): 
    tras_frente(0),
    esquerda_direita(0),
    giro_eixo(0),
    garfo(0) {}
} Velocidade;

Velocidade velocidade;

void controlarMotorDC(MotorDC *motor, int velocidade_giro) {
  if (velocidade_giro > 0) {
    digitalWrite(motor->pino_direcao_1, HIGH);
    digitalWrite(motor->pino_direcao_2, LOW);
  }

  else if (velocidade_giro < 0) {
    digitalWrite(motor->pino_direcao_1, LOW);
    digitalWrite(motor->pino_direcao_2, HIGH);
  }

  if (motor->velocidade_mov > 0 && motor->velocidade_giro == 0) {
    digitalWrite(motor->pino_direcao_1, HIGH);
    digitalWrite(motor->pino_direcao_2, LOW);
    analogWrite(motor->pino_velocidade, motor->velocidade_mov);
  }

  else if (motor->velocidade_mov < 0 && motor->velocidade_giro == 0) {
    digitalWrite(motor->pino_direcao_1, LOW);
    digitalWrite(motor->pino_direcao_2, HIGH);
    analogWrite(motor->pino_velocidade, -motor->velocidade_mov);
  }

  else if (motor->velocidade_giro > 0) {
    analogWrite(motor->pino_velocidade, motor->velocidade_giro);
  }

  else if (motor->velocidade_giro < 0) {
    analogWrite(motor->pino_velocidade, -motor->velocidade_giro);
  }

  else {
    digitalWrite(motor->pino_direcao_1, LOW);
    digitalWrite(motor->pino_direcao_1, LOW);
    analogWrite(motor->pino_velocidade, 0);
  }
}

void controlarMotorNEMA(MotorDePasso *motor, int velocidade) {
  if (velocidade > 0) {
    if (verificar_chave(chave_cima)) {
      return;
    }

    digitalWrite(motor->pino_direcao, HIGH);
  }

  else {
    if (verificar_chave(chave_baixo)) {
      return;
    }

    digitalWrite(motor->pino_direcao, LOW);
    velocidade = -velocidade;
  }

  if (velocidade != 0) {
    motor->delayStep = map(velocidade, -1000, 1000, 1000, 10);

    digitalWrite(motor->pino_pulsos, HIGH);
    delayMicroseconds(5);

    digitalWrite(motor->pino_pulsos, LOW);
    delayMicroseconds(motor->delayStep);
  }
}

bool verificar_chave(const uint8_t chave) {
  return digitalRead(chave);
}

void setup() {
  Serial.begin(115200);

  ibus.begin(Serial);

  pinMode(motor_1.pino_direcao_1, OUTPUT);
  pinMode(motor_1.pino_direcao_2, OUTPUT);
  pinMode(motor_2.pino_direcao_1, OUTPUT);
  pinMode(motor_2.pino_direcao_2, OUTPUT);
  pinMode(motor_3.pino_direcao_1, OUTPUT);
  pinMode(motor_3.pino_direcao_2, OUTPUT);
  pinMode(motor_4.pino_direcao_1, OUTPUT);
  pinMode(motor_4.pino_direcao_2, OUTPUT);

  pinMode(motor_1.pino_velocidade, OUTPUT);
  pinMode(motor_2.pino_velocidade, OUTPUT);
  pinMode(motor_3.pino_velocidade, OUTPUT);
  pinMode(motor_4.pino_velocidade, OUTPUT);

  pinMode(nema_17.pino_pulsos, OUTPUT);
  pinMode(nema_17.pino_direcao, OUTPUT);

  pinMode(chave_cima, INPUT_PULLUP);
  pinMode(chave_baixo, INPUT_PULLUP);
}

void loop() {
  velocidade.esquerda_direita = map(ibus.readChannel(0), 1000, 2000, -255, 255);
  velocidade.tras_frente = map(ibus.readChannel(1), 1000, 2000, -255, 255);
  velocidade.giro_eixo = map(ibus.readChannel(3), 1000, 2000, -255, 255);
  velocidade.garfo = map(ibus.readChannel(2), 1000, 2000, -1000, 1000);

  if (abs(velocidade.tras_frente) < 20) velocidade.tras_frente = 0;
  if (abs(velocidade.esquerda_direita) < 20) velocidade.esquerda_direita = 0;
  if (abs(velocidade.giro_eixo) < 20) velocidade.giro_eixo = 0;
  if (abs(velocidade.garfo) < 50) velocidade.garfo = 0;

  motor_1.velocidade_mov = velocidade.esquerda_direita + velocidade.tras_frente;
  motor_2.velocidade_mov = velocidade.esquerda_direita - velocidade.tras_frente;
  motor_3.velocidade_mov = -velocidade.esquerda_direita + velocidade.tras_frente;
  motor_4.velocidade_mov = -velocidade.esquerda_direita - velocidade.tras_frente;

  motor_1.velocidade_giro = velocidade.giro_eixo;
  motor_2.velocidade_giro = -velocidade.giro_eixo;
  motor_3.velocidade_giro = velocidade.giro_eixo;
  motor_4.velocidade_giro = -velocidade.giro_eixo;

  motor_1.velocidade_mov = constrain(motor_1.velocidade_mov, -255, 255);
  motor_2.velocidade_mov = constrain(motor_2.velocidade_mov, -255, 255);
  motor_3.velocidade_mov = constrain(motor_3.velocidade_mov, -255, 255);
  motor_4.velocidade_mov = constrain(motor_4.velocidade_mov, -255, 255);
  motor_1.velocidade_giro = constrain(motor_1.velocidade_giro, -255, 255);
  motor_2.velocidade_giro = constrain(motor_2.velocidade_giro, -255, 255);
  motor_3.velocidade_giro = constrain(motor_3.velocidade_giro, -255, 255);
  motor_4.velocidade_giro = constrain(motor_4.velocidade_giro, -255, 255);
  velocidade.garfo = constrain(velocidade.garfo, -1000, 1000);

  controlarMotorDC(&motor_1, velocidade.giro_eixo);
  controlarMotorDC(&motor_2, velocidade.giro_eixo);
  controlarMotorDC(&motor_3, velocidade.giro_eixo);
  controlarMotorDC(&motor_4, velocidade.giro_eixo);

  controlarMotorNEMA(&nema_17, velocidade.garfo);
}