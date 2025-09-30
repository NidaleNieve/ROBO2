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
int lastDirection;
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
  
  //emergency stop handlers
  EmergencyStop = false;
  Controller1.ButtonX.pressed(onevent_Controller1ButtonX_pressed_0);
  BumperH.pressed(onevent_BumperH_pressed_0);

  thread displaythread = thread(displayThread);

  //c 26
  //L 20 - 24
  //R 20 - 24

  //allir 94 ef að engin lína

  margin = 25;
  lastDirection = 1;
  while ((!EmergencyStop)) {
    int left  = LineTracker1.reflectivity();
    int center = LineTracker2.reflectivity();
    int right = LineTracker3.reflectivity();

    // Tveir skynjarar < 75 -> halda beinni stefnu með ±2 leiðréttingu
    if ( ((left < 75) && (center < 75)) ||
         ((center < 75) && (right < 75)) ||
         ((left < 75) && (right < 75)) ) {

      if (right + margin < left) {
        // hægri er dekkri -> smá til hægri
        LeftMotor.spin(forward, 20, percent);
        RightMotor.spin(forward, 18, percent);
      } else if (left + margin < right) {
        // vinstri er dekkri -> smá til vinstri
        LeftMotor.spin(forward, 18, percent);
        RightMotor.spin(forward, 20, percent);
      } else {
        // svipað dökkt báðum megin -> beint
        LeftMotor.spin(forward, 20, percent);
        RightMotor.spin(forward, 20, percent);
      }
      Direction = 1;
      lastDirection = 1;

    // Ef að er í miðjunni: miðja er dekkri en báðar hliðar
    } else if ((center + margin < left) && (center + margin < right)) {
      LeftMotor.spin(forward, 20, percent);
      RightMotor.spin(forward, 20, percent);
      Direction = 1;
      lastDirection = 1;

    // Ef að er hægramegin (lína vinstra megin) með smá hysteresis
    } else if (left + margin + 8 < right) {
      // Beygir til vinstri
      LeftMotor.spin(forward, 30, percent);
      RightMotor.spin(forward, 17, percent);
      Direction = 3;
      lastDirection = 3;

    // Ef að er vinstramegin (lína hægra megin) með smá hysteresis
    } else if (right + margin + 8 < left) {
      // Beygir til hægri
      LeftMotor.spin(forward, 17, percent);
      RightMotor.spin(forward, 30, percent);
      Direction = 2;
      lastDirection = 2;

    } else {
      // outside correction
      if (lastDirection == 2) {
        LeftMotor.spin(forward, 10, percent);
        RightMotor.spin(forward, 20, percent);
        Direction = 2;
      } else if (lastDirection == 3) {
        LeftMotor.spin(forward, 20, percent);
        RightMotor.spin(forward, 10, percent);
        Direction = 3;
      } else {
        LeftMotor.stop();
        RightMotor.stop();
        Direction = 0;
      }
    }
    wait(5, msec);
  }

  LeftMotor.stop();
  RightMotor.stop();
  Brain.programStop();
  return 0;

}
