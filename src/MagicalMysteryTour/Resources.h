#ifndef MAGICALMYSTERYTOUR_RESOURCES_H
#define MAGICALMYSTERYTOUR_RESOURCES_H

// This seems to ignore the additional include folder setting. Yay for 
// relative paths.
#include "../../../cinder/include/cinder/CinderResources.h"

#define RES_VERT_PROGRAM	CINDER_RESOURCE(../../resources/, MagicalMysteryTour.vert, 128, GLSL)
#define RES_FRAG_PROGRAM	CINDER_RESOURCE(../../resources/, MagicalMysteryTour.frag, 129, GLSL)

#endif // MAGICALMYSTERYTOUR_RESOURCES_H
