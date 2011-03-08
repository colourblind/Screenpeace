#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "Constants.h"
#include "Particles.h"
#include "Resources.h"

#include <deque>
#include <vector>
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
    Light() : Position(Vec3f(0, 0, 0)), Strength(1) { }
    Light(Vec3f position) : Position(position), Strength(1) { }
    Light(Vec3f position, float strength) : Position(position), Strength(strength) { }

    Vec3f Position;
    float Strength;
};

class FillrateNomNomNom : public APP_TYPE
{
public:
    FillrateNomNomNom() : nextEvent_(0) { }

    virtual void setup();
    virtual void update();
    virtual void draw();
    
private:
    gl::GlslProg program_;
    gl::Texture texture_;
    gl::VboMesh vbo_;
    ParticleSystem particleSystem_;
    float nextEvent_;
    CameraPersp camera_;
    std::deque<Light> lights_;
    Timer timer_;
};

void FillrateNomNomNom::setup()
{
    Rand::randomize();

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));

    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
    format.setMagFilter(GL_LINEAR);
    
    texture_ = gl::Texture(loadImage(loadResource(RES_PARTICLE_IMG)), format);
    texture_.enableAndBind();

    gl::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setDynamicPositions();
	layout.setStaticTexCoords2d();
	vbo_ = gl::VboMesh(NUM_PARTICLES * 4, NUM_PARTICLES * 4, layout, GL_QUADS);

    vector<uint32_t> indices;
	vector<Vec2f> texCoords;
    for (int i = 0; i < NUM_PARTICLES; i ++)
    {
        indices.push_back(i * 4 + 0);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);

        texCoords.push_back(Vec2f(0, 0));
        texCoords.push_back(Vec2f(0, 1));
        texCoords.push_back(Vec2f(1, 1));
        texCoords.push_back(Vec2f(1, 0));
    }
    vbo_.bufferIndices(indices);
    vbo_.bufferTexCoords2d(0, texCoords);

    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::enableAlphaBlending();

    Emitter emitter;
    emitter.Position = Vec3f(0, 0, 0);
    emitter.SpawnPositionVariation = Vec3f(40, 40, 40);
    emitter.MinInitialVelocity = Vec3f(0, 0, 0);
    emitter.MaxInitialVelocity = Vec3f(0, 0, 0);
    emitter.MinInitialMass = 0.5f;
    emitter.MaxInitialMass = 1.0f;
    emitter.MinInitialSize = 0.5f;
    emitter.MaxInitialSize = 15;
    emitter.InitialLife = -1;
    emitter.Frequency = 0;

    particleSystem_.SetChaos(CHAOS);
    particleSystem_.SpawnParticles(&emitter, NUM_PARTICLES);
}

void FillrateNomNomNom::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();
    particleSystem_.Update(msecs);
    
    nextEvent_ -= msecs;
    if (nextEvent_ < 0)
    {
        // Time for more spice! Add a new, powerful, short-lived repeller
        float x = Rand::randFloat(-50, 50);
        float y = Rand::randFloat(-50, 50);
        float z = Rand::randFloat(-50, 50);
        
        Attractor *attractor = new Attractor();
        attractor->Position = Vec3f(x, y, z);
        attractor->Mass = Rand::randFloat(-1.5, 3);
        //if (attractor->Mass > 0)
        //    attractor->Mass *= 0.5; // We want to weight the change toward attraction, but don't want the higher mass
        attractor->Immortal = false;
        attractor->Life = Rand::randFloat(200, 400);
        particleSystem_.AddAttractor(attractor);
        
        nextEvent_ = Rand::randFloat(250, 1500);

        if (lights_.size() > 3)
            lights_.pop_back();

        lights_.push_front(Light(attractor->Position));
    }

    for (deque<Light>::iterator iter = lights_.begin(); iter != lights_.end(); iter ++)
    {
        (*iter).Strength -= msecs / 3000; // Making this number bigger increases the falloff time of the lights
        if ((*iter).Strength < 0)
            (*iter).Strength = 0;
    }

    // Generate new vertex data
    vector<Particle *> *particleList = particleSystem_.GetParticles();
    gl::VboMesh::VertexIter vboIter = vbo_.mapVertexBuffer();
    for (vector<Particle *>::iterator iter = particleList->begin(); iter != particleList->end(); iter ++)
    {
        float size = (*iter)->Size / 2;
        Vec3f position = (*iter)->Position;
        vboIter.setPosition(position + Vec3f(-size, -size, 0));         ++ vboIter; 
        vboIter.setPosition(position + Vec3f(-size, size, 0));          ++ vboIter;
        vboIter.setPosition(position + Vec3f(size, size, 0));           ++ vboIter;
        vboIter.setPosition(position + Vec3f(size, -size, 0));          ++ vboIter;
    }
}

bool SortPredicate(Particle *a, Particle *b)
{
    return a->Position.z > b->Position.z;
}

void FillrateNomNomNom::draw()
{
    if (CLEAR_FRAMEBUFFER)
        gl::clear(Color(0, 0, 0));

    // Getting the window dimensions in setup() is bugged for AppScreenSaver
    camera_ = CameraPersp(getWindowWidth(), getWindowHeight(), 60);
    camera_.lookAt(Vec3f(0, 0, -100), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    gl::setMatrices(camera_);

    Vec3f right = Vec3f(-1, 0, 0);
    Vec3f up = camera_.getWorldUp();

    vector<Particle *> *particleList = particleSystem_.GetParticles();
    if (PERFORM_SORT)
        sort(particleList->begin(), particleList->end(), SortPredicate);

    if (SHOW_TAILS)
    {
        program_.unbind();
        texture_.disable();
        gl::color(ColorA(1, 1, 1, 0.1f));
        for (vector<Particle *>::iterator iter = particleList->begin(); iter != particleList->end(); iter ++)
        {
            gl::drawLine((*iter)->Position, (*iter)->Position - ((*iter)->Velocity * 20));
        }
    }

    program_.bind();
    texture_.enableAndBind();

    deque<Light>::iterator iter = lights_.begin();
    for (int i = 0; i < 3; i ++)
    {
        stringstream posName;
        posName << "lightPos" << i;
        if (iter != lights_.end())
        {
            program_.uniform(posName.str(), (*iter).Position);
            iter ++;
        }
        else
        {
            program_.uniform(posName.str(), Vec3f());
        }
    }

    gl::draw(vbo_);

    #ifdef _DEBUG
        stringstream fpsString;
        fpsString << "FPS:" << getAverageFps();
        program_.unbind();
        CameraOrtho textCam(0, static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()), 0, -10, 10);
        gl::setMatrices(textCam);
        gl::drawString(fpsString.str(), Vec2f(5, 5));
    #endif
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(FillrateNomNomNom, RendererGl)
#else
    CINDER_APP_BASIC(FillrateNomNomNom, RendererGl)
#endif
