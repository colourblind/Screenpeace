#ifndef DRUNKENWORMS_CONSTANTS_H
#define DRUNKENWORMS_CONSTANTS_H

const float ALPHA_DROPOFF = 0.01f;      // Rate at which the old trails fade. Higher is faster.
const int NUM_WORMS = 1024;             // Number of active worms
const bool SHOW_TRAILS = false;
const bool TRAILS_OFFSCREEN = false;    // Pings all of the trails off to the top right. Kind of hard to explain, just try it.
const int SPREAD = 4;                   // The amount of deviation from the centre. Higher numbers reduce the spread.
const float MIN_RADIUS = 5;
const float MAX_RADIUS = 40;
const float MIN_LIFE = 3000;
const float MAX_LIFE = 6000;
const float MAX_ROTATION_SPEED = 0.005f;

#endif // DRUNKENWORMS_CONSTANTS_H
