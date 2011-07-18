#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "Constants.h"
#include "Objects.h"

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
};

DeepNest::~DeepNest()
{
    for (vector<Object *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        delete *iter;
}

void DeepNest::setup()
{
    Rand::randomize();

    for (int i = 0; i < Rand::randInt(5, 10); i ++)
        objects_.push_back(ObjectFactory::CreateObject());

    gl::enableAlphaBlending();
    gl::disableDepthRead();
    gl::disableDepthWrite();
}

void DeepNest::draw()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    gl::clear();
    gl::color(Color(1, 1, 1));

    CameraPersp cam = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 0.1, 100);
    cam.lookAt(Vec3f(0, 0, 30), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    gl::setMatrices(cam);

    for (vector<Object *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        (*iter)->Update(msecs);
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(DeepNest, RendererGl)
#else
    CINDER_APP_BASIC(DeepNest, RendererGl)
#endif
