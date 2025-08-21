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

int main() {
  vexcodeInit();

  
  for (double distance_m = 0.5; distance_m <= 2.5; distance_m += 0.5) {
    double distance_mm = distance_m * 1000;  

    
    Brain.Screen.print("Keyri %.1f metra\n", distance_m);

    
    Drivetrain.driveFor(reverse, distance_mm, mm);
    Drivetrain.driveFor(forward, distance_mm, mm);

    wait(1, seconds); 
  }

  int moves = 0;

  for (int var = 1; var < 12; var += 1) {
    Drivetrain.driveFor(reverse, 0.5, m); //reverse er forward, mótarnir eru öfugir
    wait(0.3, seconds);

    if (moves == 12) { //léleg forritun en stoppa forritið ef að moves er 12
      break;
    }
    

  }
}
