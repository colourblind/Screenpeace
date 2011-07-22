#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
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
    Tri() : left_(NULL), right_(NULL), timeToLeftSpawn_(Rand::randFloat(500, 3000)), timeToRightSpawn_(Rand::randFloat(500, 3000))
    { 
        int colour = Rand::randInt(1, 8);
        float r = colour & 0x1 ? 1 : 0.75f;
        float g = colour & 0x2 ? 1 : 0.75f;
        float b = colour & 0x4 ? 1 : 0.75f;
        colour_ = Color(r, g, b);
    }
    ~Tri() { delete left_; delete right_; }
    
    void Update(float msecs);
    void Draw();

private:
    Tri *left_;
    Tri *right_;
    float timeToLeftSpawn_;
    float timeToRightSpawn_;
    Color colour_;
};

float verts[6] = { -0.5f, 0, 0.5f, 0, 0, 0.866025f };
float normals[9] = { 0, 0, 1, 0, 0, 1, 0, 0, 1 };

class TreesOfTris : public APP_TYPE
{
public:
    virtual ~TreesOfTris();

    virtual void setup();
    virtual void update();
    virtual void draw();
    
private:
    Timer timer_;
    vector<Tri *> objects_;
    gl::GlslProg program_;
};

void Tri::Update(float msecs)
{
    if (timeToLeftSpawn_ > 0)
    {
        timeToLeftSpawn_ -= msecs;
        if (timeToLeftSpawn_ <= 0)
            left_ = new Tri();
    }
    
    if (timeToRightSpawn_ > 0)
    {
        timeToRightSpawn_ -= msecs;
        if (timeToRightSpawn_ <= 0)
            right_ = new Tri();
    }

    if (left_)
        left_->Update(msecs);
    if (right_)
        right_->Update(msecs);
}

void Tri::Draw()
{
    gl::color(colour_);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glNormalPointer(GL_FLOAT, 0, normals);
	glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

    if (left_)
    {
        gl::pushModelView();
        gl::translate(Vec3f(-0.25f, math<float>::sin(M_PI / 3) * 0.5f, 0));
        gl::rotate(Vec3f(0, 0, 60));
        gl::translate(Vec3f(0, PADDING, 0));
        left_->Draw();
        gl::popModelView();
    }

    if (right_)
    {
        gl::pushModelView();
        gl::translate(Vec3f(0.25f, math<float>::sin(M_PI / 3) * 0.5f, 0));
        gl::rotate(Vec3f(0, 0, -60));
        gl::translate(Vec3f(0, PADDING, 0));
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

    objects_.push_back(new Tri());
    
    program_ = gl::GlslProg(loadResource(RES_VERT_PROGRAM), loadResource(RES_FRAG_PROGRAM));

    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void TreesOfTris::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    for (vector<Tri *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        (*iter)->Update(msecs);
}

void TreesOfTris::draw()
{
    gl::clear();

    CameraPersp cam = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 0.1, 100);
    cam.lookAt(Vec3f(0, 0, 30), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    gl::setMatrices(cam);

    program_.bind();
    program_.uniform("cameraPos", cam.getEyePoint());
    program_.uniform("lightPos", Vec3f(2, 2, 3));

    for (vector<Tri *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        (*iter)->Draw();
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(TreesOfTris, RendererGl)
#else
    CINDER_APP_BASIC(TreesOfTris, RendererGl)
#endif
