#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "Constants.h"
#include "Resources.h"

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
    Vec2f windPosition_;
    Vec2f windVelocity_;
    float currentNoise_;
};

void StrawberryFieldsForever::setup()
{
    Rand::randomize();

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));

    int octaves = static_cast<int>(math<float>::log(FIELD_SIZE) / math<float>::log(2));

    windPosition_ = Vec2f(Rand::randFloat(0, static_cast<float>(FIELD_SIZE)), Rand::randFloat(0, static_cast<float>(FIELD_SIZE)));
    windVelocity_ = Vec2f(0.1f, 0.1f);
    for (int k = 0; k < NUM_NOISES; k ++)
    {
        Perlin noise = Perlin(octaves, Rand::randInt());
        for (int i = 0; i < FIELD_SIZE; i ++)
            for (int j = 0; j < FIELD_SIZE; j ++)
                fieldKeyframes_[k][i][j] = Vec2f(0, noise.fBm(Vec2f(static_cast<float>(i), static_cast<float>(j)) / 24));
    }
    currentNoise_ = 0;
}

void StrawberryFieldsForever::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    windPosition_ += windVelocity_;
    if (windPosition_.x > FIELD_SIZE)
        windPosition_.x -= FIELD_SIZE;
    else if (windPosition_.x < 0)
        windPosition_.x += FIELD_SIZE;
    if (windPosition_.y > FIELD_SIZE)
        windPosition_.y -= FIELD_SIZE;
    else if (windPosition_.y < 0)
        windPosition_.y += FIELD_SIZE;

    currentNoise_ += 0.1f;
    if (math<float>::floor(currentNoise_) > NUM_NOISES)
        currentNoise_ = 0;           // Need to handle this properly when lerping
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
            int noiseIndex = static_cast<int>(math<float>::floor(currentNoise_));

            float a = fieldKeyframes_[noiseIndex][i][j].y;
            float b = fieldKeyframes_[(noiseIndex + 1) % NUM_NOISES][i][j].y;
            float t = (currentNoise_ - noiseIndex);

            float offset = lerp(a, b, t);

            program_.uniform("offset", Vec2f(offset / 2, offset / 2));
            gl::drawSolidRect(Rectf(i, j, i + 0.75f, j + 0.75f));
        }
    }
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(StrawberryFieldsForever, RendererGl)
#else
    CINDER_APP_BASIC(StrawberryFieldsForever, RendererGl)
#endif
