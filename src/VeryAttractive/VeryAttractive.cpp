#include "cinder/app/AppBasic.h"
#include "cinder/app/AppScreenSaver.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "Constants.h"
#include "Resources.h"

#include <vector>

using namespace std;
using namespace cinder;
using namespace cinder::app;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

// TODO: Update libcinder so I can add colour buffers to VboMesh

// According to: http://www.bentamari.com/attractors.html
// x' =  sin (ax) - z cos( by) dt
// y' = z sin (cx) - cos (dy) dt
// z' = e / sin (x) dt
// Also: http://technocosm.org/chaos/attractors.html 
// And: http://www.hiddendimension.com/Tutorials/AttractorTraps/Attractor_Traps_Tutorial_2.html

// http://www.chaoscope.org/gallery.htm
// http://blog.nihilogic.dk/2009/10/strange-attractors-beautiful-chaos-and.html
// http://www.nihilogic.dk/labs/strange_attractors/attractor.js

class Pickover : public APP_TYPE
{
public:
    virtual void setup();
    virtual void update();
    virtual void draw();
    
private:
    gl::Texture particleTexture_;
    gl::VboMesh points_;
    Timer timer_;
    float cameraRange_;
    float cameraAngle_;
};

void Pickover::setup()
{
    cameraAngle_ = 0;
    cameraRange_ = 0;
    
    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    format.setMagFilter(GL_LINEAR);
    
    particleTexture_ = gl::Texture(loadImage(loadResource(RES_PARTICLE_TEXTURE)), format);
    
    Rand::randomize();
    Perlin perlin(4);
    perlin.setSeed(Rand::randInt());

    int lightNum = Rand::randInt(3) + 1;
    Vec3f *lightPos = new Vec3f[lightNum];
    Color *lightCol = new Color[lightNum];

    for (int i = 0; i < lightNum; i ++)
    {
        lightPos[i] = Vec3f(Rand::randFloat(-1.5, 1.5), Rand::randFloat(-1.5, 1.5), Rand::randFloat(-1.5, 1.5));
        Color col = Color(Rand::randFloat(0, 1), Rand::randFloat(0, 1), Rand::randFloat(0, 1));
        lightCol[i] = col / max(col.r, max(col.g, col.b));
    }

    // Generate our parameters
    float a = 2.24f;
    float b = 0.43f;
    float c = -0.65f;
    float d = -2.43f;
    float e = 1;
    
    // Check to see if they are likely to be chaotic
    // TODO

    vector<Vec3f> p;
    vector<ColorA> colours;

    Vec3f prevPoint;
    p.push_back(prevPoint);
    colours.push_back(ColorA(1, 1, 1, ALPHA));
    for (unsigned int i = 1; i < ITERATIONS; i ++)
    {
        // Generate new point's position
        Vec3f newPoint;

        newPoint.x = sin(a * prevPoint.y) - prevPoint.z * cos(b * prevPoint.x);
        newPoint.y = prevPoint.z * sin(c * prevPoint.x) - cos(d * prevPoint.y);
        newPoint.z = e * sin(prevPoint.x);
        
        //newPoint.x = sin(a * prevPoint.x) - prevPoint.z * cos(b * prevPoint.y);
        //newPoint.y = prevPoint.z * sin(c * prevPoint.x) - cos(d * prevPoint.y);
        //newPoint.z = e / sin(prevPoint.x);

        // Keep track of the maximum range for when we set the camera up
        cameraRange_ = max(cameraRange_, newPoint.lengthSquared());

        // Apply some noise to break it up a little
        Vec3f noise = perlin.dfBm(newPoint);
        ColorA colour;
        for (int j = 0; j < lightNum; j ++)
        {
            float range = lightPos[j].distanceSquared(newPoint + noise * CHAOS) * 6;
            colour += lightCol[j] / range;
            colour.a = ALPHA;
        }
        
        p.push_back(newPoint + noise * CHAOS);
        colours.push_back(colour);
        prevPoint = newPoint;
    }

    delete[] lightPos;
    delete[] lightCol;
    
    gl::VboMesh::Layout layout;
	layout.setStaticPositions();
    layout.setStaticColorsRGBA();

    points_ = gl::VboMesh(ITERATIONS, ITERATIONS, layout, GL_POINTS);
    points_.bufferPositions(p);
    points_.bufferColorsRGBA(colours);

    cameraRange_ = ::sqrt(cameraRange_) * 2;
}

void Pickover::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    cameraAngle_ += CAMERA_SPEED * msecs;
    if (cameraAngle_ > M_PI * 2)
        cameraAngle_ -= M_PI * 2;
}

void Pickover::draw()
{
    int windowWidth = getWindowWidth();
    int windowHeight = getWindowHeight();
    CameraPersp camera(windowWidth, windowHeight, 75, 0.001f, 20);
    camera.lookAt(Vec3f(sin(cameraAngle_), 0, cos(cameraAngle_)) * 3, Vec3f(0, 0, 0));
    
    gl::clear();
    gl::setMatrices(camera);
    gl::enableAdditiveBlending();
    particleTexture_.enableAndBind();
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    gl::enable(GL_POINT_SPRITE);
    glPointSize(3.f);
    gl::drawArrays(points_, 0, ITERATIONS);
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Pickover, RendererGl)
#else
    CINDER_APP_BASIC(Pickover, RendererGl)
#endif
