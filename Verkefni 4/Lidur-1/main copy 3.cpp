/*----------------------------------------------------------------------------*/
/*  VEX V5: use LineTracker changed() callbacks instead of polling            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
using namespace vex;

bool EmergencyStop;
int  margin;
int  Direction;       //1=miðju, 2=hægrimegin, 3=vinstrimegin, 0=stopped
int  lastSide = 0;    // -1 = left, +1 = right, 0 = center

// latest sensor readings (updated by callbacks)
volatile int leftVal = 0, centerVal = 0, rightVal = 0;

// thresholds from your measurements: floor vs line midpoints
// L: floor~17 / line~7  -> 12
// C: floor~24 / line~10 -> 17
// R: floor~19 / line~8  -> 13
const int TH_L = 12, TH_C = 17, TH_R = 13;

// --- Emergency Stop triggers ---
void onevent_Controller1ButtonX_pressed_0() { EmergencyStop = true; }
void onevent_BumperH_pressed_0()           { EmergencyStop = true; }

// --- Line tracker change callbacks (just cache current values) ---
void onLeftChanged()   { leftVal   = LineTracker1.reflectivity(); }
void onCenterChanged() { centerVal = LineTracker2.reflectivity(); }
void onRightChanged()  { rightVal  = LineTracker3.reflectivity(); }

// --- Display (uses cached values) ---
void displayFunction() {
  Brain.Screen.clearLine(1);
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("L:%3d C:%3d R:%3d", leftVal, centerVal, rightVal);

  Brain.Screen.clearLine(2);
  Brain.Screen.setCursor(2, 2);
  if (Direction == 1)      Brain.Screen.print("Er í Midjunni ");
  else if (Direction == 2) Brain.Screen.print("Er Haegramegin ");
  else if (Direction == 3) Brain.Screen.print("Er Vinstramegin ");
  else                     Brain.Screen.print("Stopped ");
}

int displayThread() {
  while (true) { displayFunction(); wait(100, msec); }
  return 0;
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // Your measured notes:
  // L: 3 í loftinu, 17 á gólfi, 7 yfir línu
  // C: 4 í loftinu, 24 á gólfi, 10 yfir línu 
  // R: 3 í loftinu, 19 á gólfi, 8 yfir línu 

  // emergency stop handlers
  EmergencyStop = false;
  Controller1.ButtonX.pressed(onevent_Controller1ButtonX_pressed_0);
  BumperH.pressed(onevent_BumperH_pressed_0);

  // register changed() callbacks
  LineTracker1.changed(onLeftChanged);
  LineTracker2.changed(onCenterChanged);
  LineTracker3.changed(onRightChanged);

  // prime cached values once at start
  leftVal   = LineTracker1.reflectivity();
  centerVal = LineTracker2.reflectivity();
  rightVal  = LineTracker3.reflectivity();

  thread displaythread = thread(displayThread);

  margin = 2;

  while (!EmergencyStop) {
    // use cached readings set by changed() callbacks
    int left   = leftVal;
    int center = centerVal;
    int right  = rightVal;

    // simple per-sensor thresholds (darker -> line)
    bool L = (left   < TH_L);
    bool C = (center < TH_C);
    bool R = (right  < TH_R);

    if (C) {
      // center on line → go straight
      LeftMotor.spin(forward, 20, percent);
      RightMotor.spin(forward, 20, percent);
      Direction = 1;
      lastSide = 0;
    } else if (L) {
      // left sees line → steer left
      LeftMotor.spin(forward, 20, percent);
      RightMotor.spin(forward, 10, percent);
      Direction = 3;
      lastSide = -1;
    } else if (R) {
      // right sees line → steer right
      LeftMotor.spin(forward, 10, percent);
      RightMotor.spin(forward, 20, percent);
      Direction = 2;
      lastSide = +1;
    } else {
      // no sensor sees line → recover toward last seen side
      if (lastSide == -1) {
        LeftMotor.spin(forward, 8, percent);
        RightMotor.spin(forward, 15, percent);
      } else if (lastSide == +1) {
        LeftMotor.spin(forward, 15, percent);
        RightMotor.spin(forward, 8, percent);
      } else {
        LeftMotor.stop();
        RightMotor.stop();
        Direction = 0;
      }
    }

    wait(10, msec); // short loop delay
  }

  LeftMotor.stop();
  RightMotor.stop();
  Brain.programStop();
  return 0;
}
