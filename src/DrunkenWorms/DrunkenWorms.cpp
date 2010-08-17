#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/CinderMath.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "Constants.h"

#include <vector>

using namespace cinder;
using namespace cinder::app;
using namespace std;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

struct Worm
{
    void Init(float windowWidth, float windowHeight)
    {
        Radius = Rand::randFloat(MIN_RADIUS, MAX_RADIUS);
        RadiusChange = Rand::randFloat(-0.005f, 0.005f);
        CentreChange = Vec2f(Rand::randFloat(-0.01f, 0.01f), Rand::randFloat(-0.01f, 0.01f));
        Colour = Color(Rand::randFloat(0, 0.5f), Rand::randFloat(0, 0.25f), 0);
        Rotation = Rand::randFloat(0, static_cast<float>(M_PI * 2));
        RotationChange = Rand::randFloat(-MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
        Life = Rand::randFloat(MIN_LIFE, MAX_LIFE);

        float rot = Rand::randFloat(0, static_cast<float>(M_PI * 2));
        float distance = (windowWidth / 2) / pow(Rand::randFloat(1, SPREAD), 2);
        Centre = Vec2f(math<float>::sin(rot) * distance + (windowWidth / 2), math<float>::cos(rot) * distance + (windowHeight / 2));

        if (TRAILS_OFFSCREEN)
            LastPosition = Vec2f(-1000, -1000);

        Position = GetNextPosition();
    }

    Vec2f GetNextPosition()
    {
        return Vec2f(math<float>::sin(Rotation), math<float>::cos(Rotation)) * Radius + Centre;
    }

    float Radius;
    float RadiusChange;
    Vec2f Centre;
    Vec2f CentreChange;
    Color Colour;
    float Rotation;
    float RotationChange;
    float Life;
    Vec2f Position;
    Vec2f LastPosition;
};

class DrunkenWorms : public APP_TYPE
{
public:
    DrunkenWorms() : noSeriouslyInit_(false) { }

    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    Timer timer_;
    Worm worms_[NUM_WORMS];
    bool noSeriouslyInit_;
};

void DrunkenWorms::setup()
{
    Rand::randomize();

    gl::enableAlphaBlending();
    gl::disableDepthRead();
    gl::disableDepthWrite();
}

void DrunkenWorms::update()
{
    // Screensavers in release mode have issues getting window dimensions in
    // init. Hence this bullshit.
    if (!noSeriouslyInit_)
    {
        float windowWidth = static_cast<float>(getWindowWidth());
        float windowHeight = static_cast<float>(getWindowHeight());

        for (int i = 0; i < NUM_WORMS; i ++)
            worms_[i].Init(windowWidth, windowHeight);

        noSeriouslyInit_ = true;
    }

    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    float windowWidth = static_cast<float>(getWindowWidth());
    float windowHeight = static_cast<float>(getWindowHeight());

    for (int i = 0; i < NUM_WORMS; i ++)
    {
        worms_[i].Life -= msecs;
        if (worms_[i].Life < 0)
            worms_[i].Init(windowWidth, windowHeight);
        else
        {
            worms_[i].Rotation += worms_[i].RotationChange * msecs;
            worms_[i].Radius += worms_[i].RadiusChange * msecs;
            worms_[i].Centre += worms_[i].CentreChange * msecs;

            worms_[i].Position = worms_[i].GetNextPosition();
        }
    }
}

void DrunkenWorms::draw()
{
    float windowWidth = static_cast<float>(getWindowWidth());
    float windowHeight = static_cast<float>(getWindowHeight());

    CameraOrtho ortho = CameraOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    gl::setMatrices(ortho);
    gl::color(ColorA(1, 1, 1, ALPHA_DROPOFF));
    gl::drawSolidRect(Rectf(0, 0, windowWidth, windowHeight));

    for (int i = 0; i < NUM_WORMS; i ++)
    {
        gl::color(worms_[i].Colour);
        gl::drawLine(worms_[i].LastPosition, worms_[i].Position);
        worms_[i].LastPosition = worms_[i].Position;
    }
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(DrunkenWorms, RendererGl)
#else
    CINDER_APP_BASIC(DrunkenWorms, RendererGl)
#endif
