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
float threshold;
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

  //prenta direction
  if (Direction == 1) {
    Brain.Screen.print("Er í Miðjunni ");
  } else if (Direction == 2) {
    Brain.Screen.print("Er Hægramegin ");
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

  threshold = 30;
  while ((!EmergencyStop)) {
    bool leftOnLine = LineTracker1.reflectivity() < threshold;
    bool centerOnLine = LineTracker2.reflectivity() < threshold;
    bool rightOnLine = LineTracker3.reflectivity() < threshold;

    //Ef að er í miðjunni
    if (centerOnLine && !leftOnLine && !rightOnLine) {
      //fer áfram
      LeftMotor.spin(forward, 50, percent);
      RightMotor.spin(forward, 50, percent);
      Direction = 1;
    
    //Ef að er hægramegin
    } else if (leftOnLine) {
      //Beygir til vinstri
      LeftMotor.spin(forward, 20, percent);
      RightMotor.spin(forward, 50, percent);
      Direction = 2;

    //Ef að er vinstramegin
    } else if (rightOnLine) {
      //Beygir til hægri
      LeftMotor.spin(forward, 50, percent);
      RightMotor.spin(forward, 20, percent);
      Direction = 3;
    } else {
      //Stoppar ef engin lína er fundin
      LeftMotor.stop();
      RightMotor.stop();
    }
    wait(5, msec);
  }

  LeftMotor.stop();
  RightMotor.stop();
  Brain.programStop();
  return 0;

}
