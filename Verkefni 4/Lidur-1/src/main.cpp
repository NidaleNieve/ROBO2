#include "vex.h"
using namespace vex;

// ---- START VEXCODE CONFIGURED DEVICES ----
// LineTracker1 line A
// LineTracker2 line B
// LineTracker3 line C
// LeftMotor motor 1
// RightMotor motor 10
// Controller1, Brain, BumperH configured in GUI
// ---- END VEXCODE CONFIGURED DEVICES ----

bool EmergencyStop = false;

// Direction: 1=center, 2=right, 3=left, 0=stopped (kept from your code)
int Direction = 0;

// ====== TUNABLES ======
const int   BASE_SPEED   = 35;     // forward speed (percent)
const float KP           = 55.0f;  // proportional gain (bigger = more aggressive turning)
const float KD           = 12.0f;  // derivative gain (damps oscillation)
const float EMA_ALPHA    = 0.35f;  // sensor smoothing [0..1], higher = less smoothing
const int   MAX_TURN     = 45;     // max extra speed applied to one side
const int   RECOV_SPEED  = 20;     // spin speed while searching
const int   RECOV_TIMEOUT_MS = 900; // fail-safe per recovery attempt
const int   DARK_MIN_SUM = 30;     // if total “darkness” below this, we assume the line is lost
// ======================

// util: clamp helper
static inline float clampf(float v, float lo, float hi) {
  return (v < lo) ? lo : (v > hi) ? hi : v;
}

// emergency stop triggers
void onX()        { EmergencyStop = true; }
void onBumperH()  { EmergencyStop = true; }

// debug display
void displayFunction(int L, int C, int R, float pos, bool lost) {
  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1,1);
  Brain.Screen.print("L:%3d  C:%3d  R:%3d", L, C, R);

  Brain.Screen.setCursor(2,1);
  if (Direction == 1)      Brain.Screen.print("Dir: Midja   ");
  else if (Direction == 2) Brain.Screen.print("Dir: Haegri  ");
  else if (Direction == 3) Brain.Screen.print("Dir: Vinstri ");
  else                     Brain.Screen.print("Dir: STOP    ");

  Brain.Screen.setCursor(3,1);
  Brain.Screen.print("pos: %.2f   lost:%d", pos, lost ? 1 : 0);
}

int main() {
  vexcodeInit();

  // emergency handlers
  Controller1.ButtonX.pressed(onX);
  BumperH.pressed(onBumperH);

  // smoothed reflectivity (percent). VEX reflectivity: white = high, black = low.
  float L_ema = LineTracker1.reflectivity();
  float C_ema = LineTracker2.reflectivity();
  float R_ema = LineTracker3.reflectivity();

  // control state
  float prevError = 0.0f;
  int   lastSide  = 0;   // -1 = last seen on left, +1 = last seen on right, 0 = center/unknown
  timer recovT;

  while (!EmergencyStop) {
    // read raw
    int L_raw = LineTracker1.reflectivity();
    int C_raw = LineTracker2.reflectivity();
    int R_raw = LineTracker3.reflectivity();

    // smooth them (EMA)
    L_ema = EMA_ALPHA * L_raw + (1.0f - EMA_ALPHA) * L_ema;
    C_ema = EMA_ALPHA * C_raw + (1.0f - EMA_ALPHA) * C_ema;
    R_ema = EMA_ALPHA * R_raw + (1.0f - EMA_ALPHA) * R_ema;

    // convert to “darkness” (higher = darker) so we can weight by the line (black)
    float L_dark = 100.0f - L_ema;
    float C_dark = 100.0f - C_ema;
    float R_dark = 100.0f - R_ema;

    // total darkness — if too small, we lost the line (all bright)
    float darkSum = L_dark + C_dark + R_dark;
    bool lost = (darkSum < DARK_MIN_SUM);

    // weighted position of the line across sensors: left=-1, center=0, right=+1
    // this automatically compensates sensor offsets
    float pos = 0.0f;
    if (!lost) {
      pos = ((-1.0f * L_dark) + (0.0f * C_dark) + (1.0f * R_dark)) / darkSum;

      // remember last side for recovery: use the *edge* with more darkness if center is weak
      if (L_dark > R_dark && L_dark > C_dark * 1.05f) lastSide = -1;
      else if (R_dark > L_dark && R_dark > C_dark * 1.05f) lastSide = +1;
      else lastSide = 0; // center dominant / ambiguous — safe to reset
    }

    // compute control (P + D) on position error (target is pos=0 i.e., centered)
    float error = pos;
    float deriv = error - prevError;
    prevError = error;

    float turn = KP * error + KD * deriv;             // how much we need to yaw
    turn = clampf(turn, -MAX_TURN, MAX_TURN);

    // drive logic
    if (!lost) {
      // forward with differential
      float leftSpd  = clampf(BASE_SPEED - turn, -100, 100);
      float rightSpd = clampf(BASE_SPEED + turn, -100, 100);

      LeftMotor.spin(fwd, leftSpd, percent);
      RightMotor.spin(fwd, rightSpd, percent);

      // UI direction hint (rough)
      if (std::abs(error) < 0.15f)      Direction = 1; // midja
      else if (error > 0.0f)            Direction = 2; // right of line -> steer right wheel faster
      else                              Direction = 3; // left of line

      recovT.reset(); // cancel recovery timer while we are on the line
    } else {
      // LOST: spin toward the last known side to reacquire the line
      // fallback: if unknown, bias a tiny bit to the left (arbitrary but deterministic)
      int side = (lastSide != 0) ? lastSide : -1;

      if (side < 0) { // search to the left (spin CCW)
        LeftMotor.spin(reverse, RECOV_SPEED, percent);
        RightMotor.spin(forward, RECOV_SPEED, percent);
        Direction = 3;
      } else {        // search to the right (spin CW)
        LeftMotor.spin(forward, RECOV_SPEED, percent);
        RightMotor.spin(reverse, RECOV_SPEED, percent);
        Direction = 2;
      }

      // safety: if we spin too long without finding anything, roll forward slowly
      if (recovT.time(msec) > RECOV_TIMEOUT_MS) {
        LeftMotor.spin(fwd, 12, percent);
        RightMotor.spin(fwd, 12, percent);
        // then immediately try spinning the opposite way next time
        lastSide = -lastSide;
        recovT.reset();
      }
    }

    displayFunction((int)L_ema, (int)C_ema, (int)R_ema, pos, lost);
    wait(10, msec);
  }

  // stop cleanly
  LeftMotor.stop(coast);
  RightMotor.stop(coast);
  Brain.programStop();
  return 0;
}
