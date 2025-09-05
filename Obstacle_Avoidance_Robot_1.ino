#include <AFMotor.h>

enum TurnDir { TURN_LEFT, TURN_RIGHT };
void avoidSequence(TurnDir dir);

// ---------------- Pin Mapping ----------------
#define S1Trig A0   // Left  Trig
#define S2Trig A1   // Middle Trig
#define S3Trig A2   // Right Trig
#define S1Echo A3   // Left  Echo
#define S2Echo A4   // Middle Echo
#define S3Echo A5   // Right Echo

// ---------------- Motors ----------------
AF_DCMotor motor1(1);  // Front-Left
AF_DCMotor motor2(2);  // Back-Left
AF_DCMotor motor3(3);  // Front-Right
AF_DCMotor motor4(4);  // Back-Right

// ---------------- veriables (You Can be Edit) ----------------
const int  SPEED_FWD   = 160;   // It is the forward speed   // 0–255 
const int  SPEED_TURN  = 160;   // It is the Turn speed      // 0–255 
const int  SPEED_BACK  = 150;   // It is the backward speed  // 0–255 

const int  THRESH_CM   = 32;    // Object deect disstance (cm)
const int  SENSOR_GAP  = 50;    // Delay sensors cross active

const int  BACK_MS     = 300;   // It is the backward action time
const int  TURN_MS     = 300;   // It is the Turn action time

// ------------- Helpers -------------
long readCM(uint8_t trig, uint8_t echo) {
  digitalWrite(trig, LOW);  delayMicroseconds(4);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);

  unsigned long t = pulseIn(echo, HIGH, 30000UL);
  if (t == 0) return 999;                         
  return (long)(t / 29 / 2);                     
}

void setSpeedAll(int s) {
  motor1.setSpeed(s);
  motor2.setSpeed(s);
  motor3.setSpeed(s);
  motor4.setSpeed(s);
}

void stopAll() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void forward() {
  setSpeedAll(SPEED_FWD);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void backward() {
  setSpeedAll(SPEED_BACK);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void leftTurn() {
  setSpeedAll(SPEED_TURN);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void rightTurn() {
  setSpeedAll(SPEED_TURN);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

// Sequencewhen detect object: Stop -> 1s -> Back -> 1s -> Turn -> 1s -> Forward
void avoidSequence(TurnDir dir) {
  stopAll();           delay(1000);
  backward();          delay(BACK_MS);
  stopAll();           delay(1000);
  if (dir == TURN_LEFT)  leftTurn();
  else                   rightTurn();
  delay(TURN_MS);
  stopAll();           delay(1000);
  forward();
}

void setup() {
  pinMode(S1Trig, OUTPUT); pinMode(S1Echo, INPUT);
  pinMode(S2Trig, OUTPUT); pinMode(S2Echo, INPUT);
  pinMode(S3Trig, OUTPUT); pinMode(S3Echo, INPUT);

  forward();  
}

void loop() {
  // Avoide cross talk between the sensors
  int distL = (int)readCM(S1Trig, S1Echo); delay(SENSOR_GAP);
  int distC = (int)readCM(S2Trig, S2Echo); delay(SENSOR_GAP);
  int distR = (int)readCM(S3Trig, S3Echo); delay(SENSOR_GAP);

  bool leftHit   = (distL <= THRESH_CM);
  bool centerHit = (distC <= THRESH_CM);
  bool rightHit  = (distR <= THRESH_CM);

  if (leftHit || centerHit || rightHit) {
    if (centerHit) {
      // Left or Right turrn decision
      if (distL < distR) avoidSequence(TURN_LEFT);
      else               avoidSequence(TURN_RIGHT);
    } else if (leftHit) {
      avoidSequence(TURN_RIGHT);
    } else { // rightHit
      avoidSequence(TURN_LEFT);
    }
  } else {
    forward();
  }

  delay(30);
}
