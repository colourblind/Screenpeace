#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "Constants.h"
#include "Resources.h"

#include <list>

using namespace cinder;
using namespace cinder::app;
using namespace std;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

struct Cube
{
    Cube() { }
    Cube(Vec3f position, float size, Vec3f parentSpeed, Vec3f rotate) : Position(position), Size(size), Rotate(rotate) { InitAsplode(parentSpeed); }

    void InitAsplode(Vec3f parentSpeed)
    {
        Velocity = Vec3f(Rand::randFloat(-MAX_ASPLODE_SPEED, MAX_ASPLODE_SPEED), Rand::randFloat(-MAX_ASPLODE_SPEED, MAX_ASPLODE_SPEED), Rand::randFloat(-MAX_ASPLODE_SPEED, MAX_ASPLODE_SPEED)) * Size + parentSpeed;
        SpinSpeed = Vec3f(Rand::randFloat(-MAX_SPIN_SPEED, MAX_SPIN_SPEED), Rand::randFloat(-MAX_SPIN_SPEED, MAX_SPIN_SPEED), Rand::randFloat(-MAX_SPIN_SPEED, MAX_SPIN_SPEED));
        TimeUntilPop = Rand::randFloat(MIN_EVENT_TIME, MAX_EVENT_TIME);
    }

    Vec3f Position;
    Vec3f Velocity;
    Vec3f Rotate;
    Vec3f SpinSpeed;
    float Size;
    float TimeUntilPop;
};

class Cubesplosion : public APP_TYPE
{
public:
    ~Cubesplosion();

    virtual void setup();
    virtual void draw();
    virtual void update();

    void Reset();

private:
    list<Cube *> cubes_;
    Timer timer_;
    gl::GlslProg program_;
    float timeUntilReset_;
};

Cubesplosion::~Cubesplosion()
{
    for (list<Cube *>::iterator iter = cubes_.begin(); iter != cubes_.end(); iter ++)
        delete *iter;
}

void Cubesplosion::setup()
{
    Rand::randomize();

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));
    gl::enableDepthRead();

    Reset();
}

void Cubesplosion::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    timeUntilReset_ -= msecs;
    if (timeUntilReset_ < 0)
        Reset();

    for (list<Cube *>::iterator iter = cubes_.begin(); iter != cubes_.end(); iter ++)
    {
        Cube *currentCube = (*iter);
        currentCube->Position += currentCube->Velocity * msecs;
        currentCube->Rotate += currentCube->SpinSpeed * msecs;
        currentCube->TimeUntilPop -= msecs;

        if (currentCube->TimeUntilPop < 0 && currentCube->Size > (1.0f / (1 << DIVISIONS)))
        {
            float half = currentCube->Size / 2;
            float quarter = currentCube->Size / 4;

            Cube *a = new Cube(currentCube->Position + Vec3f(quarter, quarter, quarter), half, currentCube->Velocity, currentCube->Rotate);
            Cube *b = new Cube(currentCube->Position + Vec3f(-quarter, quarter, quarter), half, currentCube->Velocity, currentCube->Rotate);
            Cube *c = new Cube(currentCube->Position + Vec3f(quarter, -quarter, quarter), half, currentCube->Velocity, currentCube->Rotate);
            Cube *d = new Cube(currentCube->Position + Vec3f(-quarter, -quarter, quarter), half, currentCube->Velocity, currentCube->Rotate);
            Cube *e = new Cube(currentCube->Position + Vec3f(quarter, quarter, -quarter), half, currentCube->Velocity, currentCube->Rotate);
            Cube *f = new Cube(currentCube->Position + Vec3f(-quarter, quarter, -quarter), half, currentCube->Velocity, currentCube->Rotate);
            Cube *g = new Cube(currentCube->Position + Vec3f(quarter, -quarter, -quarter), half, currentCube->Velocity, currentCube->Rotate);

            currentCube->Position += Vec3f(-quarter, -quarter, -quarter);
            currentCube->Size = half;
            currentCube->InitAsplode(currentCube->Velocity);

            cubes_.push_back(a);
            cubes_.push_back(b);
            cubes_.push_back(c);
            cubes_.push_back(d);
            cubes_.push_back(e);
            cubes_.push_back(f);
            cubes_.push_back(g);
        }
    }
}

void Cubesplosion::draw()
{
    gl::clear(Color(0, 0, 0.5f));

    Vec3f cameraPos = Vec3f(0, 0, -6);

    CameraPersp persp = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 0.1f, 200);
    persp.lookAt(cameraPos, Vec3f(0, 0, 0));
    gl::setMatrices(persp);

    program_.bind();
    program_.uniform("camPos", cameraPos);

    gl::disableDepthWrite();
    program_.uniform("lightPos", Vec3f(20, -15, 25));
    program_.uniform("colour", Vec3f(0, 0, 1));
    program_.uniform("normalFlip", -1.0f);
    gl::drawCube(cameraPos, Vec3f(150, 100, 100));

    gl::enableDepthWrite();
    program_.uniform("lightPos", Vec3f(3, -1, -5));
    program_.uniform("colour", Vec3f(1, 0, 0));
    program_.uniform("normalFlip", 1.0f);
    for (list<Cube *>::iterator iter = cubes_.begin(); iter != cubes_.end(); iter ++)
    {
        gl::pushModelView();
        gl::rotate((*iter)->Rotate);
        gl::drawCube(Vec3f((*iter)->Position), Vec3f((*iter)->Size, (*iter)->Size, (*iter)->Size));
        gl::popModelView();
    }
}

void Cubesplosion::Reset()
{
    for (list<Cube *>::iterator iter = cubes_.begin(); iter != cubes_.end(); iter ++)
        delete *iter;
    cubes_.clear();

    Vec3f drift = Vec3f(Rand::randFloat(-MAX_START_SPEED, MAX_START_SPEED), Rand::randFloat(-MAX_START_SPEED, MAX_START_SPEED), Rand::randFloat(-MAX_START_SPEED, MAX_START_SPEED));
    Cube *cube = new Cube(Vec3f(0, 0, 0), 1, drift, Vec3f());
    cubes_.push_back(cube);

    timeUntilReset_ = RESET_TIME * 1000;
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Cubesplosion, RendererGl)
#else
    CINDER_APP_BASIC(Cubesplosion, RendererGl)
#endif
