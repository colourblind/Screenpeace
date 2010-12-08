Screenpeace is a collection of screensavers built using the Cinder framework.

// ****************************************************************************

CHURN
Three cubes with a noise texture that rotate around the camera, producing a 
nuseating effect best seen sober.

CUBESPLOSION
A cube that splits into progressively smaller cubes.

DRUNKEN WORMS
Hundreds of thin wormy trails that follow circular but slightly chaotic circle 
paths.

FILLRATE NOM NOM NOM
A pulsing, twitchily lit particle system.

STRAWBERRY FIELDS FOREVER
Flowing Perlin noise based fields of red polys.

TIME FOR BED SAID ZEBEDEE
Hierarchically organised spinning circles.

// ****************************************************************************

Things to be aware of:

- The project files work on the assumption that Cinder is in a directory 
alongside this one. If this isn't the case you'll have to tweak the additional 
include and library folders in each build target accordingly.

- The most obviously tweakable values are stored in a Constants.h file within 
each project.

- There are several build targets, one for making windowed apps (much easier 
to debug), one of Vista/Win7 screensavers, and one for XP screensavers, and 
release and debug for each of those.

- All of this is very much work in progress, nothing is ever finished, and 
the code is likely to contain lots of vestigial junk and blind alleys. It's 
all doodling!
