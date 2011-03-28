#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/Timer.h"
#include "cinder/TriMesh.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "Constants.h"
#include "Resources.h"

#include <vector>

using namespace cinder;
using namespace cinder::app;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

class ShinyGloop : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    Timer timer_;
    GLuint cubeMap_;
    CameraPersp camera_;
    gl::GlslProg program_;
    TriMesh mesh_;
    float heightMap_[MAP_SIZE][MAP_SIZE];
    float rotation_;
};

void ShinyGloop::setup()
{
    Rand::randomize();

    // Load cube map
    glGenTextures(1, &cubeMap_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap_);

    Surface8u imageData = Surface8u(loadImage(loadResource(RES_CUBEMAP_IMAGE)));

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0, GL_RGBA, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 0, GL_RGBA, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 0, GL_RGBA, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 0, GL_RGBA, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 0, GL_RGBA, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 0, GL_RGBA, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.getData());
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));

    camera_ = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 1, 500);
    camera_.lookAt(Vec3f(0, 0, 12), Vec3f(100, 0, 12), Vec3f(0, 0, 1));
    // camera_.lookAt(Vec3f(0, 0, 300), Vec3f(0, 0, 0), Vec3f(0, 0, 1));

    rotation_ = 100;

    gl::enableDepthRead();
    gl::enableDepthWrite();

    Perlin perlin(9);

    // Create verts
    for (int i = 0; i < MAP_SIZE; i ++)
        for (int j = 0; j < MAP_SIZE; j ++)
            mesh_.appendVertex(Vec3f(i, j, perlin.fBm(static_cast<float>(i) / MAP_SIZE, static_cast<float>(j) / MAP_SIZE) * HEIGHT_SCALE));

    // Create indices
    for (int i = 1; i < MAP_SIZE; i ++)
    {
        for (int j = 1; j < MAP_SIZE; j ++)
        {
            int index0 = (i - 1) * MAP_SIZE + (j - 1);
            int index1 = (i - 1) * MAP_SIZE + j;
            int index2 = i * MAP_SIZE + (j - 1);
            int index3 = i * MAP_SIZE + j;

            mesh_.appendTriangle(index0, index1, index2);
            mesh_.appendTriangle(index1, index2, index3);
        }
    }
}

void ShinyGloop::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    rotation_ += 0.025f * msecs;
    if (rotation_ > 360)
        rotation_ -= 360;
}

void ShinyGloop::draw()
{
    gl::clear(Color(0, 0, 0));

    gl::setMatrices(camera_);
    
    program_.bind();

    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap_);

    gl::color(ColorA(1, 1, 1, 1));
    gl::pushModelView();
    gl::rotate(Vec3f(0, 0, rotation_));
    gl::translate(Vec3f(MAP_SIZE * -0.5f, MAP_SIZE * -0.5f, 0));
    gl::draw(mesh_);
    gl::popModelView();

    //Vec3f right, up;
    //camera_.getBillboardVectors(&right, &up);
    //gl::color(Color(1, 0, 0));
    //gl::drawBillboard(Vec3f(0, 0, 0), Vec2f(256, 256), 0, right, up);
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(ShinyGloop, RendererGl)
#else
    CINDER_APP_BASIC(ShinyGloop, RendererGl)
#endif
