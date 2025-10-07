/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/*    Module:             main.cpp                                                  */
/*    Author:             VEX                                                       */
/*    Created:            Wed Jun 09 2021                                           */
/*    Description:        Drive to Location (Using Tangents)                        */
/*                        This example will show how to use a GPS Sensor to         */
/*                        navigate a V5 Moby Hero Bot to the center of the field    */
/*                        by using a tangent calculation to determine the heading   */
/*                        to drive towards                                          */
/*                                                                                  */
/*    Starting Position:  Any                                                       */
/*                                                                                  */
/*----------------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    1, 10, 2                 
// Rotation4            rotation      4               
// GPS8                 gps           9               
// DistanceLeft         distance      12              
// DistanceRight        distance      20              
// Optical19            optical       19              
// BumperA              bumper        A               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

void printPosition() {
  // Print GPS position values to the V5 Brain
  Brain.Screen.print("X: %.2f", GPS8.xPosition(mm));
  Brain.Screen.print("  Y: %.2f", GPS8.yPosition(mm));
  Brain.Screen.newLine();
}

int main() {
  // Calibrate the GPS before starting
  GPS8.calibrate();
  while (GPS8.isCalibrating()) { task::sleep(50); }

  // Orient the drivetrain's heading with the GPS heading
  Drivetrain.setHeading(GPS8.heading(), degrees);
  Drivetrain.setTurnVelocity(25, percent);

  // Print the starting position of the robot
  printPosition();

  // Store the current position of the robot
  float startingX = GPS8.xPosition(mm);
  float startingY = GPS8.yPosition(mm);

  // Target ending position
  float endingX = 0;
  float endingY = 0;

  // Tolerances (mm) to consider "close enough"
  const float tol = 5.0;

  // Drive in chunks to avoid overshoot and diagonal motion.
  const float chunk = 50.0; // mm per chunk (adjustable)
  const int maxChunks = 200; // safety cap (so we don't loop forever)

  // --- Step 1: Move along X axis to reach endingX (single drive) ---
  {
    float currentX = GPS8.xPosition(mm);
    float deltaX = endingX - currentX;
    if (fabs(deltaX) > tol) {
      // Heading: +X -> 90, -X -> 270
      float xHeading = (deltaX > 0) ? 90.0f : 270.0f;
      // Debug info
      Brain.Screen.print("DEBUG before X move -> curX: %.2f dX: %.2f xHeading: %.2f GPSheading: %.2f", currentX, deltaX, xHeading, GPS8.heading());
      Brain.Screen.newLine();
      Drivetrain.turnToHeading(xHeading, degrees, true);
      Drivetrain.driveFor(forward, fabs(deltaX), mm, true);
      task::sleep(150);
    } else {
      Brain.Screen.print("X already within tolerance\n");
    }
  }

  // Intermediate position
  printPosition();

  // --- Step 2: Move along Y axis to reach endingY (single drive) ---
  {
    float currentY = GPS8.yPosition(mm);
    float deltaY = endingY - currentY;
    if (fabs(deltaY) > tol) {
      // Heading: +Y -> 0, -Y -> 180
      float yHeading = (deltaY > 0) ? 0.0f : 180.0f;
      // Debug info
      Brain.Screen.print("DEBUG before Y move -> curY: %.2f dY: %.2f yHeading: %.2f GPSheading: %.2f", currentY, deltaY, yHeading, GPS8.heading());
      Brain.Screen.newLine();
      Drivetrain.turnToHeading(yHeading, degrees, true);
      Drivetrain.driveFor(forward, fabs(deltaY), mm, true);
      task::sleep(150);
    } else {
      Brain.Screen.print("Y already within tolerance\n");
    }
  }

  // Final position
  printPosition();
}