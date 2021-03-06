#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/ImageIO.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "Constants.h"
#include "Resources.h"

using namespace cinder;
using namespace cinder::app;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

struct SwirlBox
{
    SwirlBox();

    gl::Texture Texture;
    Vec3f Rotation;
    Vec3f RotationSpeed;
    Vec3f Colour;
};

class Churn : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    SwirlBox boxen_[NUM_BOXEN];
    Timer timer_;
};

void Churn::setup()
{
    Rand::randomize();

    for (int i = 0; i < NUM_BOXEN; i ++)
        boxen_[i] = SwirlBox();

    // Load texture
    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
    format.setMagFilter(GL_LINEAR);
    
    gl::Texture texture = gl::Texture(loadImage(loadResource(RES_CLOUD_IMG)), format);
    for (int i = 0; i < NUM_BOXEN; i ++)
    {
        boxen_[i].Texture = texture;
    }

    gl::enableAlphaBlending();
    gl::disableDepthRead();
    gl::disableDepthWrite();
}

SwirlBox::SwirlBox() : 
    Rotation(Vec3f(Rand::randFloat(0.0f, 360.0f), Rand::randFloat(0.0f, 360.0f), Rand::randFloat(0.0f, 360.0f))),
    RotationSpeed(Vec3f(Rand::randFloat(-0.075f, 0.075f), Rand::randFloat(-0.075f, 0.075f), Rand::randFloat(-0.075f, 0.075f))),
    Colour(Vec3f(Rand::randFloat(0.02f, 0.1f), Rand::randFloat(0.02f, 0.1f), Rand::randFloat(0.02f, 0.1f)))
{

}

void Churn::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    for (int i = 0; i < 3; i ++)
    {
        boxen_[i].Rotation += boxen_[i].RotationSpeed * msecs;
        
        if (boxen_[i].Rotation.x < 0)
            boxen_[i].Rotation.x += 360;
        else if (boxen_[i].Rotation.x > 360)
            boxen_[i].Rotation.x -= 360;
            
        if (boxen_[i].Rotation.y < 0)
            boxen_[i].Rotation.y += 360;
        else if (boxen_[i].Rotation.y > 360)
            boxen_[i].Rotation.y -= 360;

        if (boxen_[i].Rotation.z < 0)
            boxen_[i].Rotation.z += 360;
        else if (boxen_[i].Rotation.z > 360)
            boxen_[i].Rotation.z -= 360;
    }
}

void Churn::draw()
{
    float windowWidth = static_cast<float>(getWindowWidth());
    float windowHeight = static_cast<float>(getWindowHeight());

    CameraOrtho ortho = CameraOrtho(0, static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()), 0, -1, 1);
    gl::setMatrices(ortho);
    gl::enableAlphaBlending();
    gl::color(ColorA(0, 0, 0, ALPHA_DROPOFF));
    gl::drawSolidRect(Rectf(0, 0, windowWidth, windowHeight));

    CameraPersp persp = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 0.5, 100);
    persp.lookAt(Vec3f(0, 0, 0), Vec3f(0, 0, 1));
    gl::setMatrices(persp);
    gl::enableAdditiveBlending();
    for (int i = 0; i < NUM_BOXEN; i ++)
    {
        gl::pushModelView();

        gl::color(Color(boxen_[i].Colour.x, boxen_[i].Colour.y, boxen_[i].Colour.z));
        boxen_[i].Texture.enableAndBind();
        gl::rotate(boxen_[i].Rotation);
        gl::drawCube(Vec3f(0, 0, 0), Vec3f(10, 10, 10));
        boxen_[i].Texture.unbind();
        
        gl::popModelView();
    }
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Churn, RendererGl)
#else
    CINDER_APP_BASIC(Churn, RendererGl)
#endif
