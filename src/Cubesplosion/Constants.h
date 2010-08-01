#ifndef CUBESPLOSION_CONSTANTS_H
#define CUBESPLOSION_CONSTANTS_H

const float MAX_ASPLODE_SPEED = 0.0005f;    // maximum speed of each cube on split (scales with size)
const float MAX_START_SPEED = 0.00005f;     // maximum speed of the first cube
const float MIN_EVENT_TIME = 500;           // minimum time a cube exists before splitting
const float MAX_EVENT_TIME = 5000;          // maximum time a cube exists before splitting
const float RESET_TIME = 12;                // in seconds
const int DIVISIONS = 4;                    // number of splits

#endif // CUBESPLOSION_CONSTANTS_H
