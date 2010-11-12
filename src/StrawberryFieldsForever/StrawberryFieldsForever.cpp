#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "Constants.h"
#include "Resources.h"

#include <sstream>

using namespace cinder;
using namespace cinder::app;
using namespace std;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

class StrawberryFieldsForever : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    Timer timer_;
    gl::GlslProg program_;
    Vec2f fieldKeyframes_[NUM_NOISES][FIELD_SIZE][FIELD_SIZE];
    float timeOffset_[FIELD_SIZE][FIELD_SIZE];
    float currentNoise_;
};

float ModFloat(float a, float b)
{
    assert(a >= 0);
    if (a < b)
        return a;

    float r = a / b;
    float result = a - math<float>::floor(r) * b;
    assert(result >= 0 && result < b);
    return result;
}

void StrawberryFieldsForever::setup()
{
    Rand::randomize();

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));

    int octaves = static_cast<int>(math<float>::log(static_cast<float>(FIELD_SIZE)) / math<float>::log(2));

    for (int k = 0; k < NUM_NOISES; k ++)
    {
        Perlin noise = Perlin(octaves, Rand::randInt());
        for (int i = 0; i < FIELD_SIZE; i ++)
            for (int j = 0; j < FIELD_SIZE; j ++)
                fieldKeyframes_[k][i][j] = Vec2f(0, noise.fBm(Vec2f(static_cast<float>(i), static_cast<float>(j)) / NOISE_RESOLUTION));
    }

    Perlin noise = Perlin(octaves, Rand::randInt());
    for (int i = 0; i < FIELD_SIZE; i ++)
        for (int j = 0; j < FIELD_SIZE; j ++)
            timeOffset_[i][j] = math<float>::abs(noise.fBm(Vec2f(static_cast<float>(i), static_cast<float>(j)) / NOISE_RESOLUTION));

    currentNoise_ = 0;
}

void StrawberryFieldsForever::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    currentNoise_ += msecs * SPEED; // Will overflow eventually. Oh well.
}

void StrawberryFieldsForever::draw()
{
    gl::clear(Color(0, 0, 0));

    Vec3f cameraPos = Vec3f(0, 0, -100);

    CameraPersp persp = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 1, 300);
    persp.lookAt(cameraPos, Vec3f(0, 0, 0));
    gl::setMatrices(persp);

    gl::translate(Vec2f(FIELD_SIZE / -2, FIELD_SIZE / -2));

    program_.bind();

    program_.uniform("normalModifier", NORMAL_MODIFIER);
    program_.uniform("offsetModifier", OFFSET_MODIFIER);

    for (int i = 0; i < FIELD_SIZE; i ++)
    {
        for (int j = 0; j < FIELD_SIZE; j ++)
        {
            // This is all a cludge to stop the offsets all coming to obvious 
            // maxima and minima at the same time (as they would with just the 
            // global currentNoise_ value, and involves more maths than
            // I'd like, but hey, it works
            float noise = ModFloat(currentNoise_ + timeOffset_[i][j], static_cast<float>(NUM_NOISES));
            int noiseIndex = static_cast<int>(math<float>::floor(noise));
            assert(noiseIndex < NUM_NOISES && noiseIndex >= 0);

            float a = fieldKeyframes_[noiseIndex][i][j].y;
            float b = fieldKeyframes_[(noiseIndex + 1) % NUM_NOISES][i][j].y;
            float t = noise - noiseIndex;

            float offset = lerp(a, b, t);

            program_.uniform("offset", Vec2f(offset / 2, offset / 2));
            gl::drawSolidRect(Rectf(static_cast<float>(i), static_cast<float>(j), i + 0.75f, j + 0.75f));
        }
    }

    //gl::color(Color(1, 1, 1));
    //stringstream fpsString;
    //fpsString << "FPS:" << "??";
    //program_.unbind();
    //CameraOrtho textCam(0, static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()), 0, -10, 10);
    //gl::setMatrices(textCam);
    //gl::drawString(fpsString.str(), Vec2f(5, 5));
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(StrawberryFieldsForever, RendererGl)
#else
    CINDER_APP_BASIC(StrawberryFieldsForever, RendererGl)
#endif
