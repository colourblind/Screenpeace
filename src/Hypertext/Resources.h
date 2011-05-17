#ifndef HYPERTEXT_RESOURCES_H
#define HYPERTEXT_RESOURCES_H

// This seems to ignore the additional include folder setting. Yay for 
// relative paths.
#include "../../../cinder/include/cinder/CinderResources.h"

#define RES_CLOUD_IMG	    CINDER_RESOURCE(../../resources/, cloud0.png, 128, PNG)
#define RES_FONT_DEFINITION CINDER_RESOURCE(../../resources/, hypertext_font.fnt, 129, XML)
#define RES_FONT_IMG1       CINDER_RESOURCE(../../resources/, hypertext_font_1.png, 130, PNG)
#define RES_FONT_IMG2       CINDER_RESOURCE(../../resources/, hypertext_font_2.png, 131, PNG)
#define RES_TEXT            CINDER_RESOURCE(./, Hypertext.cpp, 132, XML)  // http://inception.davepedu.com/

#endif // HYPERTEXT_RESOURCES_H
