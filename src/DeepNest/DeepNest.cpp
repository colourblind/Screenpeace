#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "Constants.h"
#include "Objects.h"
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

class DeepNest : public APP_TYPE
{
public:
    virtual ~DeepNest();

    virtual void setup();
    virtual void draw();
    
private:
    Timer timer_;
    vector<Object *> objects_;
    gl::GlslProg program_;
};

DeepNest::~DeepNest()
{
    for (vector<Object *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        delete *iter;
}

void DeepNest::setup()
{
    Rand::randomize();

    for (int i = 0; i < Rand::randInt(10, 20); i ++)
        objects_.push_back(ObjectFactory::CreateObject());

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));

    gl::enableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::enable(GL_CULL_FACE);
}

void DeepNest::draw()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    gl::clear();

    CameraPersp cam = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 0.1, 100);
    cam.lookAt(Vec3f(0, 0, 30), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    gl::setMatrices(cam);

    program_.unbind();
    gl::color(Color(1, 1, 0));
    gl::drawCube(Vec3f(0, 0, 0), Vec3f(2, 2, 2));

    program_.bind();
    program_.uniform("cameraPos", cam.getEyePoint());
    program_.uniform("lightPos", Vec3f(0, 0, 0));
    program_.uniform("colour", Vec3f(1, 1, 1));

    for (vector<Object *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        (*iter)->Update(msecs);
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(DeepNest, RendererGl)
#else
    CINDER_APP_BASIC(DeepNest, RendererGl)
#endif
