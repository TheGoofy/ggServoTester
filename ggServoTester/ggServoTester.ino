#include <Servo.h>
#include "ggButton.h"
#include "ggSampler.h"

// #define _DEBUG_

#define LED_A_PIN 9
#define LED_B_PIN 8
#define LED_C_PIN 7
#define LED_D_PIN 6
#define LED_LIMIT_PIN 13

class ggKey : public ggButton
{
  
public:

  ggKey(byte aPin,
        boolean aInverted = false,
        boolean aEnablePullUp = false,
        float aDecaySeconds = 0.025f)
  : ggButton(aPin, aInverted, aEnablePullUp, aDecaySeconds)
  {
    SetRepeat(0.7f, 10.0f);
  }

  void SetRepeat(float aDelaySeconds,
                 float aRepeatRateHZ)
  {
    mRepeatDelayMillis = 1000.0f * aDelaySeconds;
    mRepeatIntervalMillis = 1000.0f / aRepeatRateHZ;
    mRepeatNextMillis = mRepeatDelayMillis;
  }

  boolean Pressed() const
  {
    boolean vPressed = SwitchingOn();

    long vTimeOn = GetTimeOn();
    if (vTimeOn < 0) mRepeatNextMillis = mRepeatDelayMillis;
    if (vTimeOn >= mRepeatNextMillis) {
      mRepeatNextMillis += mRepeatIntervalMillis;
      vPressed = true;
    }
    
    return vPressed;    
  }

private:

  long mRepeatDelayMillis;
  long mRepeatIntervalMillis;
  mutable long mRepeatNextMillis;
  
};

ggKey mButtonA(2, true, true);
ggKey mButtonB(3, true, true);
ggKey mButtonC(4, true, true);
ggKey mButtonD(5, true, true);

#define SERVO_PIN 10
Servo mServo;
ggSampler mSampler(300.0f);

const int mAngleCenter = 90;
const int mAngleDeltaMin = -60;
const int mAngleDeltaMax = 60;
const int mAngleStep = 4;

int mAngle = mAngleCenter;
int mAngleSet = mAngleCenter;

void SetLEDs(bool aA, bool aB, bool aC, bool aD, bool aLimit)
{
  digitalWrite(LED_A_PIN, aA);
  digitalWrite(LED_B_PIN, aB);
  digitalWrite(LED_C_PIN, aC);
  digitalWrite(LED_D_PIN, aD);
  digitalWrite(LED_LIMIT_PIN, aLimit);
}

void DisplayAngle()
{
  if      (mAngle <= mAngleCenter+3*mAngleDeltaMin/3) SetLEDs(1, 0, 0, 0, 1);
  else if (mAngle <  mAngleCenter+2*mAngleDeltaMin/3) SetLEDs(1, 0, 0, 0, 0);
  else if (mAngle <  mAngleCenter+1*mAngleDeltaMin/3) SetLEDs(1, 1, 0, 0, 0);
  else if (mAngle <  mAngleCenter                   ) SetLEDs(0, 1, 0, 0, 0);
  else if (mAngle == mAngleCenter                   ) SetLEDs(0, 1, 1, 0, 1);
  else if (mAngle <  mAngleCenter+1*mAngleDeltaMax/3) SetLEDs(0, 0, 1, 0, 0);
  else if (mAngle <  mAngleCenter+2*mAngleDeltaMax/3) SetLEDs(0, 0, 1, 1, 0);
  else if (mAngle <  mAngleCenter+3*mAngleDeltaMax/3) SetLEDs(0, 0, 0, 1, 0);
  else                                                SetLEDs(0, 0, 0, 1, 1);
  #ifdef _DEBUG_
  Serial.println(mAngle);
  #endif
}

void setup()
{
  #ifdef _DEBUG_
  Serial.begin(115200);
  #endif
  pinMode(LED_LIMIT_PIN, OUTPUT);
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(LED_C_PIN, OUTPUT);
  pinMode(LED_D_PIN, OUTPUT);
  mButtonA.begin();
  mButtonB.begin();
  mButtonC.begin();
  mButtonD.begin();
  mServo.attach(SERVO_PIN);
  mServo.write(mAngle);
  DisplayAngle();
}

void loop()
{
  if (mButtonA.Pressed()) mAngleSet = (mAngleSet <= mAngleCenter) ? mAngleCenter + mAngleDeltaMin : mAngleCenter;
  if (mButtonB.Pressed()) mAngleSet = ((mAngleSet - mAngleStep) >= (mAngleCenter + mAngleDeltaMin)) ? (mAngleSet - mAngleStep) : mAngleSet;
  if (mButtonC.Pressed()) mAngleSet = ((mAngleSet + mAngleStep) <= (mAngleCenter + mAngleDeltaMax)) ? (mAngleSet + mAngleStep) : mAngleSet;
  if (mButtonD.Pressed()) mAngleSet = (mAngleSet >= mAngleCenter) ? mAngleCenter + mAngleDeltaMax : mAngleCenter;
  /*
  if (mAngle != mAngleSet) {
    mAngle = mAngleSet;
    mServo.write(mAngle);
    DisplayAngle();
  }
  */
  if (mSampler.IsDue()) {
    int vAngleOld = mAngle;
    if (mAngleSet > mAngle) mAngle++;
    if (mAngleSet < mAngle) mAngle--;
    if (mAngle != vAngleOld) {
      mServo.write(mAngle);
      DisplayAngle();
    }
  }
}
