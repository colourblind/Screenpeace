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

class MagicalMysteryTour : public APP_TYPE
{
public:
    MagicalMysteryTour() : initialised_(false), rotation_(Vec3f(0, 0, 0)) { }

    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    void GenerateMesh(TriMesh *mesh, int level, Vec3f centre);

    bool initialised_;
    Timer timer_;
    CameraPersp cam_;
    gl::GlslProg program_;
    gl::GlslProg final_;
    gl::Fbo fbo_;
    TriMesh mesh_;
    Vec3f rotation_;
    Vec3f cameraPosition_;
};

void MagicalMysteryTour::setup()
{
    Rand::randomize();

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));
    final_ = gl::GlslProg(loadResource(RES_VERT_DEFAULT), loadResource(RES_FRAG_MAGIC));
    
    GenerateMesh(&mesh_, 1, Vec3f(0, 0, 0));
}

void MagicalMysteryTour::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    //cameraPosition_ += Vec3f(-0.0002f, 0, 0) * msecs;
    rotation_ += Vec3f(0, 0.13f, 0);
}

void MagicalMysteryTour::draw()
{
    int windowWidth = getWindowWidth();
    int windowHeight = getWindowHeight();

    // Since fetching window dimensions fails for screensavers during setup(), lazy create the FBO and camera
    if (!initialised_)
    {
        cam_ = CameraPersp(windowWidth, windowHeight, 60, 0.01f, 6);
        cameraPosition_ = Vec3f(2, -0.5f, 0.5);
        fbo_ = gl::Fbo(windowWidth, windowHeight, false, true, true);
        initialised_ = true;
    }

    fbo_.bindFramebuffer();
    gl::clear(Color(1, 1, 1));
	gl::enableDepthRead();
    gl::enableDepthWrite();

    cam_.lookAt(cameraPosition_, Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    gl::setMatrices(cam_); 

    program_.bind();

    gl::pushModelView();
    gl::rotate(rotation_);
    gl::draw(mesh_);
    gl::popModelView();

    gl::rotate(rotation_ + Vec3f(180, 0, 0));
    gl::translate(Vec3f(0, 0, 1));
    gl::draw(mesh_);

    program_.unbind();

    fbo_.unbindFramebuffer();

    // Redraw FBO back to screen
    glEnable(GL_TEXTURE_2D);

    gl::setMatricesWindow(getWindowSize());
    gl::disableDepthRead();

    // TODO: TBD post-processing magic
    fbo_.getTexture().bind(1);
    final_.bind();
    gl::draw(fbo_.getTexture(), Rectf(0, static_cast<float>(windowHeight), static_cast<float>(windowWidth), 0));
    final_.unbind();
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
