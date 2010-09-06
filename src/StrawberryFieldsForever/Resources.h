#ifndef STRAWBERRYFIELDSFOREVER_RESOURCES_H
#define STRAWBERRYFIELDSFOREVER_RESOURCES_H

// This seems to ignore the additional include folder setting. Yay for 
// relative paths.
#include "../../../cinder/include/cinder/CinderResources.h"

#define RES_VERT_PROGRAM	CINDER_RESOURCE(../../resources/, StrawberryFieldsForever.vert, 128, GLSL)
#define RES_FRAG_PROGRAM	CINDER_RESOURCE(../../resources/, StrawberryFieldsForever.frag, 129, GLSL)

#endif // STRAWBERRYFIELDSFOREVER_RESOURCES_H
