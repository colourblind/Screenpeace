#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/TriMesh.h"
#include "cinder/Vector.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "Constants.h"
#include "Resources.h"

using namespace cinder;
using namespace cinder::app;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

class RandomCam : public CameraPersp
{
public:
    RandomCam() : initialised_(false), up_(Vec3f(0, 1, 0)), targetLocked_(false) { }
    RandomCam(int width, int height) : CameraPersp(width, height, 60, 0.01f, 6), initialised_(true), up_(Vec3f(0, 1, 0)) { }

    void SetMatrices();
    void Update(float msecs);

    void SetPosition(Vec3f position) { position_ = position; }
    void SetTarget(Vec3f target) { target_ = target; }
    void SetDirection(Vec3f direction) { direction_ = direction; }
    void SetUp(Vec3f up) { up_ = up; }
    void SetLocked(bool targetLocked) { targetLocked_ = targetLocked; }

    bool IsInitialised() const { return initialised_; }

private:
    bool initialised_;

    Vec3f position_;
    Vec3f target_;
    Vec3f direction_;
    Vec3f up_;
    bool targetLocked_;
};

class MagicalMysteryTour : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    void GenerateMesh(TriMesh *mesh, int level, Vec3f centre);

    Timer timer_;
    RandomCam cam_;
    gl::GlslProg program_;
    gl::Fbo fbo_;
    TriMesh mesh_;
};

void RandomCam::SetMatrices()
{
    if (targetLocked_)
        lookAt(position_, target_, up_);
    else
        lookAt(position_, position_ + direction_, up_);
    gl::setMatrices(*this); 
}

void RandomCam::Update(float msecs)
{
    position_ += Vec3f(-0.0002f, 0, 0) * msecs;
}

void MagicalMysteryTour::setup()
{
    Rand::randomize();

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));
    
    GenerateMesh(&mesh_, 1, Vec3f(0, 0, 0));
}

void MagicalMysteryTour::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    cam_.Update(msecs);
}

void MagicalMysteryTour::draw()
{
    int windowWidth = getWindowWidth();
    int windowHeight = getWindowHeight();

    // Since fetching window dimensions fails for screensavers during setup(), lazy create the FBO and camera
    if (!cam_.IsInitialised())
    {
        cam_ = RandomCam(windowWidth, windowHeight);
        cam_.SetPosition(Vec3f(3, -0.25f, 0));
        cam_.SetDirection(Vec3f(-1, 0, 0));
        cam_.SetLocked(false);
        fbo_ = gl::Fbo(windowWidth, windowHeight, false, true, true);
    }

    fbo_.bindFramebuffer();
    gl::clear(Color(1, 1, 1));
	gl::enableDepthRead();
    gl::enableDepthWrite();

    cam_.SetMatrices();

    program_.bind();
    gl::draw(mesh_);
    program_.unbind();

    fbo_.unbindFramebuffer();

    // Redraw FBO back to screen
    glEnable(GL_TEXTURE_2D);

    gl::setMatricesWindow(getWindowSize());
    gl::disableDepthRead();

    // TODO: TBD post-processing magic
    gl::draw(fbo_.getTexture(), Rectf(0, static_cast<float>(windowHeight), static_cast<float>(windowWidth), 0));
}

// Generate mesh for Sierpinski Pyramid
void MagicalMysteryTour::GenerateMesh(TriMesh *mesh, int level, Vec3f centre)
{
    float centreYOffset = math<float>::tan(static_cast<float>(M_PI) / 6) * (1.0f / math<float>::pow(2, static_cast<float>(level))) * 2;
    float centreXOffset = (1.0f / math<float>::pow(2, static_cast<float>(level)));
    if (level < MAX_LEVELS)
    {
        // Recurse to the next level down
        GenerateMesh(mesh, level + 1, centre + Vec3f(0, centreYOffset, 0));
        GenerateMesh(mesh, level + 1, centre + Vec3f(centreXOffset, -centreYOffset, centreXOffset));
        GenerateMesh(mesh, level + 1, centre + Vec3f(centreXOffset, -centreYOffset, -centreXOffset));
        GenerateMesh(mesh, level + 1, centre + Vec3f(-centreXOffset, -centreYOffset, -centreXOffset));
        GenerateMesh(mesh, level + 1, centre + Vec3f(-centreXOffset, -centreYOffset, centreXOffset));
    }
    else
    {
        // Actually add the relevant vertices to the mesh
        size_t offset = mesh->getNumVertices();
        centreXOffset *= 2;
        centreYOffset *= 2;

        mesh->appendVertex(centre + Vec3f(0, centreYOffset, 0));
        mesh->appendVertex(centre + Vec3f(centreXOffset, -centreYOffset, centreXOffset));
        mesh->appendVertex(centre + Vec3f(centreXOffset, -centreYOffset, -centreXOffset));
        mesh->appendVertex(centre + Vec3f(-centreXOffset, -centreYOffset, -centreXOffset));
        mesh->appendVertex(centre + Vec3f(-centreXOffset, -centreYOffset, centreXOffset));

        mesh->appendNormal(Vec3f(0, 1, 0).normalized());
        mesh->appendNormal(Vec3f(1, 0, 1).normalized());
        mesh->appendNormal(Vec3f(1, 0, -1).normalized());
        mesh->appendNormal(Vec3f(-1, 0, -1).normalized());
        mesh->appendNormal(Vec3f(-1, 0, 1).normalized());

        mesh->appendTriangle(offset + 0, offset + 1, offset + 2);
        mesh->appendTriangle(offset + 0, offset + 2, offset + 3);
        mesh->appendTriangle(offset + 0, offset + 3, offset + 4);
        mesh->appendTriangle(offset + 0, offset + 4, offset + 1);
        mesh->appendTriangle(offset + 1, offset + 2, offset + 3);
        mesh->appendTriangle(offset + 2, offset + 3, offset + 4);

        /*
        // No luck with face normals for indexed meshes. Leaving this here in case I work it out later
        float foo = math<float>::sin(static_cast<float>(M_PI) / 3);
        mesh->appendNormal(Vec3f(1, foo, 0).normalized());
        mesh->appendNormal(Vec3f(0, foo, -1).normalized());
        mesh->appendNormal(Vec3f(-1, foo, 0).normalized());
        mesh->appendNormal(Vec3f(0, foo, 1).normalized());
        mesh->appendNormal(Vec3f(0, -1, 0).normalized());
        mesh->appendNormal(Vec3f(0, -1, 0).normalized());
        */
    }
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(MagicalMysteryTour, RendererGl)
#else
    CINDER_APP_BASIC(MagicalMysteryTour, RendererGl)
#endif
