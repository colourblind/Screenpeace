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
    Ribbon() : Velocity(Vec3f(0, 0, 0.0001f))
    {
        Position.push_front(Vec3f(Rand::randFloat(-15, 15), Rand::randFloat(-15, 15), Rand::randFloat(-15, 15)));
    }

    Vec3f Velocity;
    deque<Vec3f> Position;
};

struct Predator
{
    Predator() : Position(Vec3f(Rand::randFloat(-15, 15), Rand::randFloat(-15, 15), Rand::randFloat(-15, 15))), 
                    Velocity(Vec3f(0, 0, 0.0001f)),
                    Target(Vec3f(Rand::randFloat(-5, 5), Rand::randFloat(-5, 5), Rand::randFloat(-5, 5))),
                    TimeToRetarget(4000)
    { }

    Vec3f Position;
    Vec3f Velocity;
    Vec3f Target;
    float TimeToRetarget;
};

class Ribbons : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    vector<Ribbon *> ribbons_;
    Predator predator_;
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

    predator_.Velocity += (predator_.Position - predator_.Target).normalized() * -0.0005f;
    if (predator_.Velocity.lengthSquared() > 0.0001f)
        predator_.Velocity = predator_.Velocity.normalized() * 0.01f;
    predator_.Position += predator_.Velocity * msecs;
    predator_.TimeToRetarget -= msecs;
    if (predator_.Position.distanceSquared(predator_.Target) < 0.25f || predator_.TimeToRetarget < 0)
    {
        predator_.Target = Vec3f(Rand::randFloat(-8, 8), Rand::randFloat(-8, 8), Rand::randFloat(-8, 8));
        predator_.TimeToRetarget = 4000;
    }

    int i = 0;
    for (vector<Ribbon *>::iterator iter = ribbons_.begin(); iter != ribbons_.end(); iter ++, i ++)
    {
        // Trim tail
        Ribbon *current = (*iter);
        current->Position.push_front(current->Position[0]);
        if (current->Position.size() > TAIL_LENGTH)
            current->Position.pop_back();

        // Resolve forces
        Vec3f force = Vec3f::zero();
        for (int j = 0; j < NUM_RIBBONS; j ++)
        {
            if (current == ribbons_[j])
                continue;

            float distSquared = current->Position[0].distanceSquared(ribbons_[j]->Position[0]);
            if (distSquared > IGNORE_DISTANCE * IGNORE_DISTANCE)
                continue;

            Vec3f dir = (current->Position[0] - ribbons_[j]->Position[0]).normalized();

            if (distSquared > OPTIMAL_DISTANCE * OPTIMAL_DISTANCE)
            {
                float dist = ::sqrt(distSquared);
                force += dir * (math<float>::cos((dist - OPTIMAL_DISTANCE) * 2 * static_cast<float>(M_PI) / (IGNORE_DISTANCE - OPTIMAL_DISTANCE)) * -1 + 1);
                // attract
            }
            else
            {
                force += dir * ((OPTIMAL_DISTANCE * OPTIMAL_DISTANCE) / distSquared - 1) * -1 * REPELL_MODIFIER;
                // repell
            }
        }

        if (force.length() > 0)
            force.normalize();
        force += current->Position[0] * CENTRE_PULL;   // Pull back towards centre

        if (current->Position[0].distanceSquared(predator_.Position) < 49)
        {
            Vec3f predDir = (current->Position[0] - predator_.Position).normalized();
            force += predDir * -4.0f;
        }

        current->Velocity += force * -1 * msecs * 0.00001f;
        float speedSq = current->Velocity.lengthSquared();
        if (speedSq > RIBBON_MAX_SPEED * RIBBON_MAX_SPEED)
            current->Velocity = current->Velocity.normalized() * RIBBON_MAX_SPEED;
        else if (speedSq < RIBBON_MIN_SPEED * RIBBON_MIN_SPEED)
            current->Velocity = current->Velocity.normalized() * RIBBON_MIN_SPEED;
        current->Position[0] += current->Velocity * msecs;
    }
}

void Ribbons::draw()
{
    gl::clear(Color(0, 0, 0.5f));

    Vec3f cameraPos = Vec3f(0, 0, -20);

    CameraPersp persp = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 0.1f, 200);
    persp.lookAt(cameraPos, Vec3f::zero());
    gl::setMatrices(persp);

    if (SHOW_PREDATOR)
    {
        gl::color(Color(1, 0, 0));
        gl::drawCube(predator_.Position, Vec3f(1, 1, 1));
        gl::color(Color(0, 1, 0));
        gl::drawCube(predator_.Target, Vec3f(1, 1, 1));
    }

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
