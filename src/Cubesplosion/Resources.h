#ifndef CUBESPLOSION_RESOURCES_H
#define CUBESPLOSION_RESOURCES_H

// This seems to ignore the additional include folder setting. Yay for 
// relative paths.
#include "../../../cinder/include/cinder/CinderResources.h"

#define RES_VERT_PROGRAM	CINDER_RESOURCE(../../resources/, Cubesplosion.vert, 128, GLSL)
#define RES_FRAG_PROGRAM	CINDER_RESOURCE(../../resources/, Cubesplosion.frag, 129, GLSL)

#endif // CUBESPLOSION_RESOURCES_H
