#ifndef FILLRATENOMNOMNOM_RESOURCES_H
#define FILLRATENOMNOMNOM_RESOURCES_H

// This seems to ignore the additional include folder setting. Yay for 
// relative paths.
#include "../../../cinder/include/cinder/CinderResources.h"

#define RES_VERT_PROGRAM	CINDER_RESOURCE(../../resources/, FillrateNomNomNom.vert, 128, GLSL)
#define RES_FRAG_PROGRAM	CINDER_RESOURCE(../../resources/, FillrateNomNomNom.frag, 129, GLSL)
#define RES_PARTICLE_IMG	CINDER_RESOURCE(../../resources/, FillrateNomNomNom.png, 130, PNG)

#endif // FILLRATENOMNOMNOM_RESOURCES_H
