#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "Constants.h"
#include "Resources.h"

#include <vector>

using namespace std;
using namespace cinder;
using namespace cinder::app;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

class Tri
{
public:
    Tri(int level) : level_(level), left_(NULL), right_(NULL), 
        timeToLeftSpawn_(Rand::randFloat(-500 * level_, 2000)), 
        timeToRightSpawn_(Rand::randFloat(-500 * level_, 2000)),
        angle_(-179),
        targetAngle_(Rand::randFloat(-30, 30))
    { 
        int colour = Rand::randInt(1, 8);
        float r = (colour & 0x1 ? 1 : 0.75f) * TINT[0];
        float g = (colour & 0x2 ? 1 : 0.75f) * TINT[1];
        float b = (colour & 0x4 ? 1 : 0.75f) * TINT[2];
        colour_ = Color(r, g, b);
        if (level == 0)
            angle_ = targetAngle_;
        pattern_ = Rand::randInt(3);
    }
    ~Tri() { delete left_; delete right_; }
    
    bool Update(float msecs);
    void Draw();

private:
    int level_;
    Tri *left_;
    Tri *right_;
    float timeToLeftSpawn_;
    float timeToRightSpawn_;
    float targetAngle_;
    float angle_;
    Color colour_;
    int pattern_;
};

float verts[6] = { -0.5f, 0, 0.5f, 0, 0, 0.866025f };
float normals[9] = { 0, 0, 1, 0, 0, 1, 0, 0, 1 };
float texCoords[6] = { 0, 0, 5, 0, 2.5f, 8 };
gl::Texture patterns[3];

class TreesOfTris : public APP_TYPE
{
public:
    virtual ~TreesOfTris();

    virtual void setup();
    virtual void update();
    virtual void draw();
    
private:
    void draw(Camera *cam);
    void CreateTree(int index);

    Timer timer_;
    vector<Tri *> objects_;
    vector<Vec3f> translations_;
    gl::GlslProg program_;
    gl::GlslProg fadeNear_;
    gl::GlslProg fadeFar_;
    gl::GlslProg blur_[2];
    gl::Fbo far_;
    gl::Fbo near_;
    gl::Fbo pingPong_[2];
    float cameraAngle_;
    bool initialised_;
};

bool Tri::Update(float msecs)
{
    bool stagnant = true;

    if (targetAngle_ > 100 && angle_ < targetAngle_) // collapsing
    {
        stagnant = false;
        angle_ += SPAWN_SPEED * msecs;
    }
    else if (angle_ < targetAngle_) // expanding
    {
        stagnant = false;
        angle_ += SPAWN_SPEED * msecs;
    }
    else
    {
        if (timeToLeftSpawn_ > 0)
        {
            stagnant = false;
            timeToLeftSpawn_ -= msecs;
            if (timeToLeftSpawn_ <= 0)
                left_ = new Tri(level_ + 1);
        }
        
        if (timeToRightSpawn_ > 0)
        {
            stagnant = false;
            timeToRightSpawn_ -= msecs;
            if (timeToRightSpawn_ <= 0)
                right_ = new Tri(level_ + 1);
        }
    }

    if (left_)
        stagnant = left_->Update(msecs) && stagnant;
    if (right_)
        stagnant = right_->Update(msecs) && stagnant;

    // start the collapse
    if (stagnant && targetAngle_ < 100)
    {
        targetAngle_ = 179;
        stagnant = false;
    }

    return stagnant;
}

void Tri::Draw()
{
    gl::color(colour_);

    gl::translate(Vec3f(0, PADDING / 2, 0));
    gl::rotate(Vec3f(angle_, 0, 0));
    gl::translate(Vec3f(0, PADDING / 2, 0));

    patterns[pattern_].enableAndBind();

    if (angle_ < 179)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, verts);
        glNormalPointer(GL_FLOAT, 0, normals);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	    glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
	    glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (left_)
    {
        gl::pushModelView();
        gl::translate(Vec3f(-0.25f, math<float>::sin(M_PI / 3) * 0.5f, 0));
        gl::rotate(Vec3f(0, 0, 60));
        left_->Draw();
        gl::popModelView();
    }

    if (right_)
    {
        gl::pushModelView();
        gl::translate(Vec3f(0.25f, math<float>::sin(M_PI / 3) * 0.5f, 0));
        gl::rotate(Vec3f(0, 0, -60));
        right_->Draw();
        gl::popModelView();
    }
}

TreesOfTris::~TreesOfTris()
{
    for (vector<Tri *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        delete *iter;
}

void TreesOfTris::setup()
{
    Rand::randomize();

    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
    format.setMagFilter(GL_LINEAR);
    format.setWrapS(GL_REPEAT);
    format.setWrapT(GL_REPEAT);
    
    initialised_ = false;

    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));
    blur_[0] = gl::GlslProg(loadResource(RES_VERT_DEFAULT), loadResource(RES_FRAG_BLUR_H));
    blur_[1] = gl::GlslProg(loadResource(RES_VERT_DEFAULT), loadResource(RES_FRAG_BLUR_V));
    fadeNear_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_FADE_NEAR));
    fadeFar_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_FADE_FAR));
    patterns[0] = gl::Texture(loadImage(loadResource(RES_NOPATTERN)), format);
    patterns[1] = gl::Texture(loadImage(loadResource(RES_PATTERN0)), format);
    patterns[2] = gl::Texture(loadImage(loadResource(RES_PATTERN1)), format);

    cameraAngle_ = 0;

    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::enableAlphaBlending();
}

void TreesOfTris::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    for (int i = 0; i < objects_.size(); i ++)
    {
        if (objects_[i]->Update(msecs))
            CreateTree(i);
    }

    if (ANIMATE_CAMERA)
    {
        cameraAngle_ += CAMERA_SPEED * msecs;
        if (cameraAngle_ > M_PI * 2)
            cameraAngle_ -= M_PI * 2;
    }
}

void TreesOfTris::draw()
{
    if (!initialised_)
    {
        for (int i = 0; i < NUM_TREES; i ++)
            CreateTree(-1);

        int windowWidth = getWindowWidth();
        int windowHeight = getWindowHeight();

        near_ = gl::Fbo(windowWidth, windowHeight, true, true);

        pingPong_[0] = gl::Fbo(windowWidth, windowHeight, true, true, false);
        pingPong_[1] = gl::Fbo(windowWidth, windowHeight, true, true, false);

        initialised_ = true;
    }

    CameraPersp cam(getWindowWidth(), getWindowHeight(), 60, 0.1, 50);
    cam.lookAt(Vec3f(sin(cameraAngle_) * CAMERA_RANGE, 0, cos(cameraAngle_) * CAMERA_RANGE), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    CameraOrtho ortho(0, pingPong_[0].getWidth(), 0, pingPong_[0].getHeight(), -1, 1);

    near_.bindFramebuffer();
    gl::clear();

    fadeNear_.bind();
    cam.setNearClip(5);
    cam.setFarClip(CAMERA_RANGE);
    draw(&cam);

    fadeFar_.bind();
    cam.setNearClip(CAMERA_RANGE);
    cam.setFarClip(CAMERA_RANGE * 2);
    draw(&cam);

    pingPong_[0].bindFramebuffer();
    gl::clear();
    gl::setMatrices(ortho);
    blur_[0].bind();
    blur_[0].uniform("textureWidth", (float)near_.getWidth());
    near_.getTexture().bind();
    gl::drawSolidRect(Rectf(0, 0, pingPong_[0].getWidth(), pingPong_[0].getHeight()));

    pingPong_[1].bindFramebuffer();
    gl::clear();
    gl::setMatrices(ortho);
    blur_[1].bind();
    blur_[1].uniform("textureHeight", (float)near_.getHeight());
    pingPong_[0].getTexture().bind();
    gl::drawSolidRect(Rectf(0, 0, pingPong_[1].getWidth(), pingPong_[1].getHeight()));

    pingPong_[1].unbindFramebuffer();

    gl::clear();

    program_.bind();
    gl::setMatrices(cam);
    draw(&cam);
    program_.unbind();

    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    pingPong_[1].getTexture().bind();
    gl::color(Color::white());
    gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
}

void TreesOfTris::draw(Camera *cam)
{
    program_.uniform("cameraPos", cam->getEyePoint());
    program_.uniform("lightPos", Vec3f(20, 20, 30));

    gl::setMatrices(*cam);

    for (int i = 0; i < objects_.size(); i ++)
    {
        gl::pushModelView();
        gl::translate(translations_[i]);
        objects_[i]->Draw();
        gl::popModelView();
    }
}

void TreesOfTris::CreateTree(int index)
{
    Tri *t = new Tri(0);
    float aspectRatio = getWindowAspectRatio();
    Vec3f translate = Vec3f(Rand::randFloat(-8, 8) * aspectRatio, Rand::randFloat(-8, 8), Rand::randFloat(-8, 8) * aspectRatio);

    if (index < 0)
    {
        objects_.push_back(t);
        translations_.push_back(translate);
    }
    else
    {
        delete objects_[index];
        objects_[index] = t;
        translations_[index] = translate;
    }
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(TreesOfTris, RendererGl)
#else
    CINDER_APP_BASIC(TreesOfTris, RendererGl)
#endif
