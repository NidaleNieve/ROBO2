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

  for (int var = 1, int moves = 0; moves >= 12; var += 1, moves += 1) { //stilli upp breytur
    Drivetrain.driveFor(forward, 0.5, m);
    wait(0.3, seconds);

    if (var == 7) { //snúi við
      Drivetrain.turnFor(right, 90, degrees); 
      wait(0.3, seconds);

      Drivetrain.driveFor(forward, 0.5, m);
      wait(0.3, seconds);
      
      Drivetrain.turnFor(right, 90, degrees); 
      wait(0.3, seconds);

      var = 11; //set varið á 11 þannig þetta if fer ekki í gang aftur
    }

    //Aðal if fallið, sér til um í hvaða átt að beygja
    if (var % 4 == 1 || var % 4 == 0) {
      Drivetrain.turnFor(right, 90, degrees);
    } else {
      Drivetrain.turnFor(left, 90, degrees);
    }

    moves += 1; //bæti við moves til að vita hvenær vélmennið er komið
  }
  
  Drivetrain.driveFor(forward, 0.5, m); //keyri síðustu 50cm 
}
