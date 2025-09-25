/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*   This program will have the robot move left when it senses higher         */
/*   reflectivity than the threshold (Light maximum reflectivity +            */
/*   Dark maximum reflectivity / 2) and will move to the right when it        */
/*   senses less reflectivity than the threshold.                             */
/*                                                                            */
/*   IMPORTANT: In order for this program to work correctly, the Line         */
/*   Tracker needs to be mounted facing down towards the ground, close        */
/*   to the ground.                                                           */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// LineTrackerA         line          A
// LeftMotor            motor         1
// RightMotor           motor         10
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

bool EmergencyStop;
int margin;
int Direction; //1=miðju, 2=hægrimegin, 3=vinstrimegin, 0=stopped

//Emergency Stop triggers
void onevent_Controller1ButtonX_pressed_0() {
  EmergencyStop = true;
}
void onevent_BumperH_pressed_0() {
  EmergencyStop = true;
}

void displayFunction() {
  Brain.Screen.clearLine(1);
  Brain.Screen.setCursor(1, 1);

  Brain.Screen.print("L:%3d C:%3d R:%3d",
                      LineTracker1.reflectivity() /*+ 3*/,
                      LineTracker2.reflectivity() /*- 5*/,
                      LineTracker3.reflectivity()
                    );


  Brain.Screen.clearLine(2);
  Brain.Screen.setCursor(2, 2);
  //prenta direction
  if (Direction == 1) {
    Brain.Screen.print("Er í Midjunni ");
  } else if (Direction == 2) {
    Brain.Screen.print("Er Haegramegin ");
  } else if (Direction == 3) {
    Brain.Screen.print("Er Vinstramegin ");
  } else {
    Brain.Screen.print("Stopped ");
  }
}

int displayThread() {
  while (true) {
    displayFunction();
    wait(100, msec);
  }
  return 0;
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  

  //L: 3 í loftinu, 17 á gólfi, 7 yfir línu
  //C: 4 í loftinu, 24 á gólfi, 10 yfir línu 
  //R: 3 í loftinu, 19 á gólfi, 8 yfir línu 

  //emergency stop handlers
  EmergencyStop = false;
  Controller1.ButtonX.pressed(onevent_Controller1ButtonX_pressed_0);
  BumperH.pressed(onevent_BumperH_pressed_0);

  thread displaythread = thread(displayThread);


  int baseL = 17, baseC = 24, baseR = 19; // drifting ground baselines
  const int offL = 7,  offC = 10,  offR = 8; // half of (ground - line) per sensor
  const int emaN = 10; // EMA factor: 1/emaN update each loop

  margin = 2;
  int lastSide = 0; // -1 = left, +1 = right, 0 = center

  while (!EmergencyStop) {
    
    int left   = LineTracker1.reflectivity();
    int center = LineTracker2.reflectivity();
    int right  = LineTracker3.reflectivity();

    // compute dynamic thresholds from drifting baselines
    int thL = baseL - (offL - margin);
    int thC = baseC - (offC - margin);
    int thR = baseR - (offR - margin);

    // decide if each sensor is on the line (darker)
    bool L = (left < thL);
    bool C = (center < thC);
    bool R = (right < thR);

    /*
    // if a sensor is probably on floor (not line), update its baseline a little
    if (!L) baseL = (baseL * (emaN - 1) + left)   / emaN;
    if (!C) baseC = (baseC * (emaN - 1) + center) / emaN;
    if (!R) baseR = (baseR * (emaN - 1) + right)  / emaN;
    */

    if (C) {
      // center on line → go straight
      LeftMotor.spin(forward, 20, percent);
      RightMotor.spin(forward, 20, percent);
      Direction = 1;
      lastSide = 0;
    }
    else if (L) {
      // left sees line → steer left
      LeftMotor.spin(forward, 20, percent);
      RightMotor.spin(forward, 10, percent);
      Direction = 3;
      lastSide = -1;
    }
    else if (R) {
      // right sees line → steer right
      LeftMotor.spin(forward, 10, percent);
      RightMotor.spin(forward, 20, percent);
      Direction = 2;
      lastSide = 1;
    }
    else {
      // no sensor sees line → recover
      if (lastSide == -1) {
        // last seen left → turn left in place
        LeftMotor.spin(forward, 8, percent);
        RightMotor.spin(forward, 15, percent);
      } else if (lastSide == 1) {
        // Beygji til hægri
        LeftMotor.spin(forward, 15, percent);
        RightMotor.spin(forward, 8, percent);
      } else {
        // never seen → stop
        LeftMotor.stop();
        RightMotor.stop();
        Direction = 0;
      }
    }

    wait(10, msec);
  }

  LeftMotor.stop();
  RightMotor.stop();
  Brain.programStop();
  return 0;

}
