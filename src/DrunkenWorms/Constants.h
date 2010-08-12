#ifndef DRUNKENWORMS_CONSTANTS_H
#define DRUNKENWORMS_CONSTANTS_H

const float ALPHA_DROPOFF = 0.01f;      // Rate at which the old trails fade. Higher is faster.
const int NUM_WORMS = 1024;             // Number of active worms
const bool TRAILS_OFFSCREEN = false;    // Pings all of the trails off to the top right. Kind of hard to explain, just try it.
const int SPREAD = 4;                   // The amount of deviation from the centre. Higher numbers reduce the spread.

#endif // DRUNKENWORMS_CONSTANTS_H
