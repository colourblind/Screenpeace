#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "Constants.h"

#include <vector>
#include <deque>

using namespace cinder;
using namespace cinder::app;
using namespace std;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

struct Ribbon
{
    Ribbon() : Velocity(Vec3f(0, 0, 0))
    {
        Position.push_front(Vec3f(Rand::randFloat(-10, 10), Rand::randFloat(-10, 10), Rand::randFloat(-10, 10)));
    }

    Vec3f Velocity;
    deque<Vec3f> Position;
};

class Ribbons : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    vector<Ribbon *> ribbons_;
    Vec3f cameraTarget_;
    Timer timer_;
};

void Ribbons::setup()
{
    Rand::randomize();

    for (int i = 0; i < NUM_RIBBONS; i ++)
        ribbons_.push_back(new Ribbon());

    gl::enableAlphaBlending();
}

void Ribbons::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    cameraTarget_ = Vec3f(0, 0, 0);

    int i = 0;
    for (vector<Ribbon *>::iterator iter = ribbons_.begin(); iter != ribbons_.end(); iter ++, i ++)
    {
        Ribbon *current = (*iter);
        current->Position.push_front(current->Position[0]);
        if (current->Position.size() > TAIL_LENGTH)
            current->Position.pop_back();

        Vec3f force = Vec3f(0, 0, 0);
        for (int j = 0; j < NUM_RIBBONS; j ++)
        {
            if (current == ribbons_[j])
                continue;
            Vec3f diff = current->Position[0] - ribbons_[j]->Position[0];
            float distance = diff.length() - OPTIMAL_DISTANCE;
            float tweakedForce = distance * distance * distance;
            Vec3f dir = diff.normalized();
            force += dir * tweakedForce;
        }

        current->Velocity += force.normalized() * -1 * RIBBON_ACCEL * msecs;
        if (current->Velocity.length() > 0)
            current->Velocity.normalize();
        current->Position[0] += current->Velocity * RIBBON_SPEED * msecs;
     
        cameraTarget_ += current->Position[0] * Vec3f(1, 1, 0);
    }
    cameraTarget_ /= NUM_RIBBONS;
}

void Ribbons::draw()
{
    gl::clear(Color(0, 0, 0.5f));

    Vec3f cameraPos = Vec3f(0, 0, -20);

    CameraPersp persp = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 0.1f, 200);
    // persp.lookAt(cameraPos, cameraTarget_);
    // persp.lookAt(ribbons_[0]->Position[0], cameraTarget_);
    persp.lookAt(ribbons_[0]->Position[0], ribbons_[0]->Position[0] + ribbons_[0]->Velocity);
    gl::setMatrices(persp);

    for (vector<Ribbon *>::iterator iter = ribbons_.begin(); iter != ribbons_.end(); iter ++)
    {
        Ribbon *current = (*iter);
        int size = current->Position.size();
        for (int i = 0; i < size - 1; i ++)
        {
            float alpha = 1.0f - (static_cast<float>(i) / static_cast<float>(size));
            gl::color(ColorA(1.0f, 1.0f, 1.0f, alpha));
            gl::drawLine(current->Position[i], current->Position[i + 1]);
        }
    }
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Ribbons, RendererGl)
#else
    CINDER_APP_BASIC(Ribbons, RendererGl)
#endif
