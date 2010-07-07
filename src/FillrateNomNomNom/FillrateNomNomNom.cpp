#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "Particles.h"
#include "Resources.h"

#include <deque>
#include <sstream>

using namespace cinder;
using namespace cinder::app;
using namespace std;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

struct Light
{
    Light() { Position = Vec3f(0, 0, 0); Strength = 0; }
    Light(Vec3f position, float strength) { Position = position; Strength = strength; }

    Vec3f Position;
    float Strength;
};

class FillrateNomNomNom : public APP_TYPE
{
public:
    FillrateNomNomNom() : nextEvent_(100) { }

    virtual void setup();
    virtual void update();
    virtual void draw();
    
private:
    gl::GlslProg program_;
    gl::Texture texture_;
    ParticleSystem particleSystem_;
    float nextEvent_;
    CameraPersp camera_;
    std::deque<Light> lights_;
};

void FillrateNomNomNom::setup()
{
    Rand::randomize();

    setFrameRate(60);

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));

    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
    format.setMagFilter(GL_LINEAR);
    
    texture_ = gl::Texture(loadImage(loadResource(RES_PARTICLE_IMG)), format);
    texture_.enableAndBind();
    gl::disableAlphaTest();
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::enableAlphaBlending();

    Emitter emitter;
    emitter.Position = Vec3f(0, 0, 0);
    emitter.SpawnPositionVariation = Vec3f(40, 40, 40);
    emitter.MinInitialVelocity = Vec3f(0, 0, 0);
    emitter.MaxInitialVelocity = Vec3f(0, 0, 0);
    emitter.MinInitialMass = 0.5f;
    emitter.MaxInitialMass = 2.5f;
    emitter.MinInitialSize = 1;
    emitter.MaxInitialSize = 20;
    emitter.InitialLife = -1;
    emitter.Frequency = 0;
    particleSystem_.SpawnParticles(&emitter, 1000);
}

void FillrateNomNomNom::update()
{
    float msecs = 1000.0f / 60;
    particleSystem_.Update(msecs);
    
    nextEvent_ -= msecs;
    if (nextEvent_ < 0)
    {
        // Time for more spice! Add a new, powerful, short-lived repeller
        float x = Rand::randFloat(-25, 25);
        float y = Rand::randFloat(-25, 25);
        float z = Rand::randFloat(-25, 25);
        
        Attractor *attractor = new Attractor();
        attractor->Position = Vec3f(x, y, z);
        attractor->Mass = Rand::randFloat(-8, 16);
        attractor->Immortal = false;
        attractor->Life = Rand::randFloat(50, 100);
        particleSystem_.AddAttractor(attractor);
        
        nextEvent_ = Rand::randFloat(250, 1000);

        if (lights_.size() > 3)
            lights_.pop_back();

        lights_.push_front(Light(attractor->Position, 0.5));
    }

    for (deque<Light>::iterator iter = lights_.begin(); iter != lights_.end(); iter ++)
    {
        (*iter).Strength -= msecs / 3000; // Making this number bigger increases the falloff time of the lights
        if ((*iter).Strength < 0)
            (*iter).Strength = 0;
    }
}

bool SortPredicate(Particle *a, Particle *b)
{
    return a->Position.z > b->Position.z;
}

void FillrateNomNomNom::draw()
{
    gl::clear(Color(0, 0, 0.));

    // Getting the window dimensions in setup() is bugged for AppScreenSaver
    camera_ = CameraPersp(getWindowWidth(), getWindowHeight(), 60);
    camera_.lookAt(Vec3f(0, 0, -100), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    gl::setMatrices(camera_);

    Vec3f right = Vec3f(-1, 0, 0);
    Vec3f up = camera_.getWorldUp();

    program_.bind();

    deque<Light>::iterator iter = lights_.begin();
    for (int i = 0; i < 4; i ++)
    {
        stringstream posName;
        posName << "lightPos" << i;
        stringstream strName;
        strName << "lightStr" << i;
        string foo = strName.str();
        if (iter != lights_.end())
        {
            program_.uniform(posName.str(), (*iter).Position);
            program_.uniform(strName.str(), (*iter).Strength); 
            iter ++;
        }
        else
        {
            program_.uniform(posName.str(), Vec3f());
            program_.uniform(strName.str(), 0); 
        }
    }

    vector<Particle *> *particleList = particleSystem_.GetParticles();
    sort(particleList->begin(), particleList->end(), SortPredicate);
    for (vector<Particle *>::iterator iter = particleList->begin(); iter != particleList->end(); iter ++)
    {
        gl::drawBillboard((*iter)->Position, Vec2f((*iter)->Size, (*iter)->Size), 0, right, up);
    }
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(FillrateNomNomNom, RendererGl)
#else
    CINDER_APP_BASIC(FillrateNomNomNom, RendererGl)
#endif
