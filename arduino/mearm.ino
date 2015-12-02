#include <Servo.h>
#include "../calibration.hh"
#include "../controller.hh"
#include "../curve.hh"

class Controller: public ControllerBase
{
public:
  Controller(void) {}
  void setup(void) {
    for (int drive=0; drive<DRIVES; drive++) {
      m_curve[drive].setBound(BOUND);
      m_servo[drive].attach(SERVOPIN[drive]);
    };
  }
  float limitArmDrive(int drive, float target) {
    return limitArm(drive, target, m_curve[SHOULDER].target(), m_curve[ELBOW].target());
  }
  void update(int dt) {
    for (int drive=0; drive<DRIVES; drive++) {
      float pwm = angleToPWM(drive, m_curve[drive].update(dt));
      m_servo[drive].writeMicroseconds(round(pwm));
    };
  };
  int offset(int drive) { return OFFSET[drive]; }
  float resolution(int drive) { return RESOLUTION[drive]; }
  int lower(int drive) { return MIN[drive]; }
  int upper(int drive) { return MAX[drive]; }
  void reportTime(void) {
    Serial.print(millis() * 0.001);
    Serial.write("\r\n");
  }
  void reportAngle(float angle) {
    Serial.print(angle);
    Serial.write("\r\n");
  }
  void reportPWM(float pwm) {
    Serial.print(round(pwm));
    Serial.write("\r\n");
  }
  void targetAngle(int drive, float target) {
    targetPWM(drive, angleToPWM(drive, target));
  }
  void stopDrives(void)
  {
    for (int drive=0; drive<DRIVES; drive++)
      m_curve[drive].stop();
  }
protected:
  Servo m_servo[DRIVES];
};

unsigned long t0;

Controller controller;

void setup() {
  controller.setup();
  Serial.begin(9600);
  t0 = millis();
}

void loop() {
  int dt = millis() - t0;
  if (Serial.available())
    controller.parseChar(Serial.read());
  controller.update(dt);
  t0 += dt;
}
