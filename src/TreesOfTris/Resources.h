#ifndef TREESOFTRIS_RESOURCES_H
#define TREESOFTRIS_RESOURCES_H

// This seems to ignore the additional include folder setting. Yay for 
// relative paths.
#include "../../../cinder/include/cinder/CinderResources.h"

#define RES_VERT_PROGRAM	CINDER_RESOURCE(../../resources/, TreesOfTris.vert, 128, GLSL)
#define RES_FRAG_PROGRAM	CINDER_RESOURCE(../../resources/, TreesOfTris.frag, 129, GLSL)
#define RES_NOPATTERN       CINDER_RESOURCE(../../resources/, blank.png, 130, PNG)
#define RES_PATTERN0        CINDER_RESOURCE(../../resources/, Pattern0.png, 131, PNG)
#define RES_PATTERN1        CINDER_RESOURCE(../../resources/, Pattern1.png, 132, PNG)

#endif // TREESOFTRIS_RESOURCES_H
