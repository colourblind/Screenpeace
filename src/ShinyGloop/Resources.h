#ifndef SHINYGLOOP_RESOURCES_H
#define SHINYGLOOP_RESOURCES_H

// This seems to ignore the additional include folder setting. Yay for 
// relative paths.
#include "../../../cinder/include/cinder/CinderResources.h"

#define RES_VERT_PROGRAM	CINDER_RESOURCE(../../resources/, ShinyGloop.vert, 128, GLSL)
#define RES_FRAG_PROGRAM	CINDER_RESOURCE(../../resources/, ShinyGloop.frag, 129, GLSL)
#define RES_CUBEMAP_IMAGE   CINDER_RESOURCE(../../resources/, ShinyGloop.png, 130, PNG)

#endif // SHINYGLOOP_RESOURCES_H
