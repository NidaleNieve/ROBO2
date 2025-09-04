/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Wed Sep 25 2019                                           */
/*    Description:  Moving Forward (mm)                                       */
/*                                                                            */
/*    This Program drives the robot forward for 150 millimeters.              */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    1, 10, D
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

volatile bool emergencyStop = false; // shared variable (read by multiple threads)

// Emergency stop thread function
int checkEmergencyStop() {
  while(true) {
    if (Controller1.ButtonX.pressing()) {
      emergencyStop = true;
      Drivetrain.stop(); // stop immediately
      Brain.Screen.printAt(10, 50, "EMERGENCY STOP PRESSED!");
    }
    wait(20, msec); // small delay to avoid hogging CPU
  }
  return 0;
}

int main() {
  // Start the emergency stop thread
  thread t1 = thread(checkEmergencyStop);

  for (double distance_m = 0.5; distance_m <= 2.5; distance_m += 0.5) {
    if (emergencyStop) break; // check before moving

    double distance_mm = distance_m * 1000;  
    Brain.Screen.print("Keyri %.1f metra\n", distance_m);

    Drivetrain.driveFor(reverse, distance_mm, mm);
    if (emergencyStop) break;

    Drivetrain.driveFor(forward, distance_mm, mm);
    if (emergencyStop) break;

    wait(1, seconds); 
  }

  // If emergency stop was triggered, halt program execution forever
  if (emergencyStop) {
    Drivetrain.stop();
    Brain.Screen.printAt(10, 70, "PROGRAM HALTED.");
    while (true) {
      exit(0); // halt program
    }
  }
}