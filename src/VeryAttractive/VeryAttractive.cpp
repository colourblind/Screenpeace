#include "cinder/app/AppBasic.h"
#include "cinder/app/AppScreenSaver.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
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
    void DrawBillboard(Vec3f objectPos, Vec2f scale, Camera *camera);

    vector<Vec3f> points_;
    gl::Texture particleTexture_;
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
    
    Perlin perlin(3);

    // Generate our parameters
    float a = 2.24f;
    float b = 0.43f;
    float c = -0.65f;
    float d = -2.43f;
    float e = 1;

   // float a = 2.0f;
  	//float b = 0.5f;
  	//float c = -1.0f;
  	//float d = -1.0f;
  	//float e = 2.0f;
    
    // Check to see if they are likely to be chaotic
    // TODO
    
    Vec3f prevPoint;
    points_.push_back(prevPoint);
    for (int i = 0; i < ITERATIONS; i ++)
    {
        // Generate new point's position
        Vec3f newPoint;
        //newPoint.x = sin(a * prevPoint.x) - prevPoint.z * cos(b * prevPoint.y);
        //newPoint.y = prevPoint.z * sin(c * prevPoint.x) - cos(d * prevPoint.y);
        //newPoint.z = e / sin(prevPoint.x);

        newPoint.x = sin(a * prevPoint.y) - prevPoint.z * cos(b * prevPoint.x);
        newPoint.y = prevPoint.z * sin(c * prevPoint.x) - cos(d * prevPoint.y);
        newPoint.z = e * sin(prevPoint.x);
        
        // Apply some noise to break it up a little
        newPoint += perlin.dfBm(newPoint * 0.01f) * 0.0025f;
        
        // Keep track of the maximum range for when we set the camera up
        cameraRange_ = max(cameraRange_, newPoint.lengthSquared());

        points_.push_back(newPoint);
        prevPoint = newPoint;
    }
    
    cameraRange_ = ::sqrt(cameraRange_) * 2;
}

void Pickover::update()
{
    float msecs = 16.67f;
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
    gl::color(ColorA(0.1f, 0.1f, 0.1f, 1));
    for (vector<Vec3f>::iterator iter = points_.begin(); iter != points_.end(); iter ++)
    {
//        gl::drawCube(*iter, Vec3f(0.01f, 0.01f, 0.01f));
//        gl::drawBillboard(*iter, Vec2f(0.2f, 0.2f), 0, Vec3f(1, 0, 0), Vec3f(0, 1, 0));
        DrawBillboard(*iter, Vec2f(0.2f, 0.2f), &camera);
    }
}

void Pickover::DrawBillboard(Vec3f objectPos, Vec2f scale, Camera *camera)
{
    // Yoinked from the SlimDX implementation (thanks Promit and friends!)
    Matrix44f result;
    Vec3f difference = objectPos - camera->getEyePoint();
    Vec3f crossed;
    Vec3f final;

    float lengthSq = difference.lengthSquared();
    if (lengthSq < 0.0001f)
        difference = camera->getViewDirection();
    else
        difference *= static_cast<float>(1.0f / math<float>::sqrt(lengthSq));

    crossed = camera->getWorldUp().cross(difference);
    crossed.normalize();
    final = difference.cross(crossed);

    result.m[0] = crossed.x;
    result.m[1] = crossed.y;
    result.m[2] = crossed.z;
    result.m[3] = 0;

    result.m[4] = final.x;
    result.m[5] = final.y;
    result.m[6] = final.z;
    result.m[7] = 0;
    
    result.m[8] = difference.x;
    result.m[9] = difference.y;
    result.m[10] = difference.z;
    result.m[11] = 0;

    result.m[12] = objectPos.x;
    result.m[13] = objectPos.y;
    result.m[14] = objectPos.z;
    result.m[15] = 1;

    glEnableClientState(GL_VERTEX_ARRAY);
    Vec3f verts[4];
    glVertexPointer(3, GL_FLOAT, 0, &verts[0].x);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

    gl::pushModelView();
    gl::multModelView(result);

    Vec2f halfScale = scale * 0.5;

    verts[0] = Vec3f(-halfScale.x, -halfScale.y, 0);
    verts[1] = Vec3f(-halfScale.x, halfScale.y, 0);
    verts[2] = Vec3f(halfScale.x, -halfScale.y, 0);
    verts[3] = Vec3f(halfScale.x, halfScale.y, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    gl::popModelView();
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Pickover, RendererGl)
#else
    CINDER_APP_BASIC(Pickover, RendererGl)
#endif
