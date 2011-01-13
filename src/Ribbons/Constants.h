#ifndef RIBBONS_CONSTANTS_H
#define RIBBONS_CONSTANTS_H

#ifdef _DEBUG
const int NUM_RIBBONS = 50;
#else
const int NUM_RIBBONS = 650;
#endif
const int TAIL_LENGTH = 10;
const float CENTRE_PULL = 0.004f;
const float RIBBON_MIN_SPEED = 0.001f;
const float RIBBON_MAX_SPEED = 0.005f;
const float OPTIMAL_DISTANCE = 3.0f;
const float IGNORE_DISTANCE = 8.0f;
const float REPELL_MODIFIER = 40.0f;
const bool SHOW_PREDATOR = false;

#endif // RIBBONS_CONSTANTS_H
