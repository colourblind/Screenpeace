#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "Constants.h"

#include <vector>

using namespace std;
using namespace cinder;
using namespace cinder::app;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

void DrawArray(vector<Vec3f> *verts, vector<ColorA> *colours, GLenum drawType, float rotation);

class Renderable
{
public:
    Renderable() : 
        rotation_(0),
        rotationSpeed_(Rand::randFloat(-MAX_ROTATION_SPEED, MAX_ROTATION_SPEED))
    { }

    virtual void Render() = 0;
    virtual void Update(float msecs)
    {
        rotation_ += rotationSpeed_ * msecs;
        if (rotation_ < 0)
            rotation_ += M_PI * 2;
        if (rotation_ > M_PI * 2)
            rotation_ -= M_PI * 2;
    }

protected:
    float rotation_;
    float rotationSpeed_;
};

class Segment : public Renderable
{
public:
    Segment() : 
        radius_(Rand::randFloat(1, 10)),
        startAngle_(Rand::randFloat(0, M_PI * 2)),
        angle_(Rand::randFloat(MIN_SEGMENT_LENGTH, MAX_SEGMENT_LENGTH)),
        startColour_(ColorA(Rand::randFloat(0, 0.5f), Rand::randFloat(0, 0.5f), 1), Rand::randFloat(0.5f, 1)),
        endColour_(ColorA(Rand::randFloat(0, 0.5f), Rand::randFloat(0, 0.5f), 1), Rand::randFloat(0.5f, 1)),
        filled_(Rand::randBool())
    {
        thickness_ = Rand::randFloat(0.1f, radius_ / 8);
        slices_ = static_cast<int>(angle_ * CURVE_FIDELITY);
    }
    
    void SetRadius(float radius) { radius_ = radius; }
    void SetStartAngle(float startAngle) { startAngle_ = startAngle; }
    void SetAngle(float angle) { angle_ = angle; }
    virtual void Render();

protected:
    float radius_;
    float thickness_;
    float startAngle_;
    float angle_;
    ColorA startColour_;
    ColorA endColour_;
    int slices_;
    bool filled_;
};

class Points : public Renderable
{
public:
    Points()
    {
        for (int i = 0; i < Rand::randInt(1, 20); i ++)
        {
            float angle = Rand::randFloat(0, M_PI * 2);
            float radius = Rand::randFloat(1, 10);
            points_.push_back(Vec3f(sin(angle) * radius, cos(angle) * radius, 0));
        }
    }
    virtual void Render();

protected:
    vector<Vec3f> points_;
};

class Trail : public Renderable
{
public:
    Trail() :
        radius_(Rand::randFloat(1, 10)),
        startAngle_(Rand::randFloat(0, M_PI * 2)),
        angle_(Rand::randFloat(MIN_SEGMENT_LENGTH, MAX_SEGMENT_LENGTH))
    {
        slices_ = static_cast<int>(angle_ * CURVE_FIDELITY);
    }
    virtual void Render();

protected:
    float radius_;
    float startAngle_;
    float angle_;
    int slices_;    
};

class Oculus : public APP_TYPE
{
public:
    virtual ~Oculus();

    virtual void setup();
    virtual void update();
    virtual void draw();
    
private:
    vector<Renderable *> objects_;
    Timer timer_;
};

void Segment::Render()
{
    vector<Vec3f> verts;
    vector<ColorA> colours;

    float halfThick = thickness_ / 2;
    float angleStep = angle_ / slices_;

    if (filled_)
    {
        for (int i = 0; i < slices_; i ++)
        {
            float currentAngle = startAngle_ + angleStep * i;
            verts.push_back(Vec3f(sin(currentAngle) * (radius_ - halfThick), cos(currentAngle) * (radius_ - halfThick), 0));
            verts.push_back(Vec3f(sin(currentAngle) * (radius_ + halfThick), cos(currentAngle) * (radius_ + halfThick), 0));
            verts.push_back(Vec3f(sin(currentAngle + angleStep) * (radius_ + halfThick), cos(currentAngle + angleStep) * (radius_ + halfThick), 0));
            verts.push_back(Vec3f(sin(currentAngle + angleStep) * (radius_ - halfThick), cos(currentAngle + angleStep) * (radius_ - halfThick), 0));
            
            colours.push_back(startColour_ + (endColour_ - startColour_) * i / slices_);
            colours.push_back(startColour_ + (endColour_ - startColour_) * i / slices_);
            colours.push_back(startColour_ + (endColour_ - startColour_) * (i + 1) / slices_);
            colours.push_back(startColour_ + (endColour_ - startColour_) * (i + 1) / slices_);
        }
    }
    else
    {
        for (int i = 0; i < slices_; i ++)
        {
            float currentAngle = startAngle_ + angleStep * i;
            verts.push_back(Vec3f(sin(currentAngle) * (radius_ - halfThick), cos(currentAngle) * (radius_ - halfThick), 0));
            colours.push_back(startColour_ + (endColour_ - startColour_) * i / slices_);
        }
        for (int i = slices_ - 1; i >= 0; i --)
        {
            float currentAngle = startAngle_ + angleStep * i;
            verts.push_back(Vec3f(sin(currentAngle) * (radius_ + halfThick), cos(currentAngle) * (radius_ + halfThick), 0));
            colours.push_back(startColour_ + (endColour_ - startColour_) * i / slices_);
        }
    }

    ::DrawArray(&verts, &colours, filled_ ? GL_QUADS : GL_LINE_LOOP, rotation_);
}

void Points::Render()
{
    vector<Vec3f> verts;
    vector<ColorA> colours;

    for (int i = 0; i < points_.size(); i ++)
    {
        verts.push_back(Vec3f(points_[i]));
        colours.push_back(ColorA(1, 1, 1, 1));
    }

    ::DrawArray(&verts, &colours, GL_POINTS, rotation_);
}

void Trail::Render()
{
    vector<Vec3f> verts;
    vector<ColorA> colours;

    float angleStep = angle_ / slices_;
    for (int i = 0; i < slices_; i ++)
    {
        float currentAngle = startAngle_ + angleStep * i;
        verts.push_back(Vec3f(sin(currentAngle) * radius_, cos(currentAngle) * radius_, 0));
        if (rotationSpeed_ > 0)
            colours.push_back(ColorA(1, 1, 1, 1 - static_cast<float>(i) / slices_));
        else
            colours.push_back(ColorA(1, 1, 1, static_cast<float>(i) / slices_));
    }

    ::DrawArray(&verts, &colours, GL_LINE_STRIP, rotation_);
}

Oculus::~Oculus()
{
    for (vector<Renderable *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        delete *iter;
    objects_.clear();
}

void Oculus::setup()
{
    Rand::randomize();

    for (int i = 0; i < NUM_SEGMENTS; i ++)
    {
        int type = Rand::randInt(5);
        switch (type)
        {
        case 0:
            objects_.push_back(new Points());
            break;
        case 1:
            objects_.push_back(new Trail());
            break;
        default:
            objects_.push_back(new Segment());
            break;
        }
    }

    gl::enableAlphaBlending();
}

void Oculus::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    for (vector<Renderable *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        (*iter)->Update(msecs);
}

void Oculus::draw()
{
    gl::clear();

    float windowWidth = static_cast<float>(getWindowWidth());
    float windowHeight = static_cast<float>(getWindowHeight());

    CameraOrtho ortho = CameraOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    gl::setMatrices(ortho);
    gl::translate(Vec2f(windowWidth / 2, windowHeight / 2));
    float scale = min((windowWidth - 100) / 20, (windowHeight - 100) / 20);
    gl::scale(Vec3f(scale, scale, 1));

    for (vector<Renderable *>::iterator iter = objects_.begin(); iter != objects_.end(); iter ++)
        (*iter)->Render();
}

void DrawArray(vector<Vec3f> *verts, vector<ColorA> *colours, GLenum drawType, float rotation)
{
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &(verts->at(0).x));
	glColorPointer(4, GL_FLOAT, 0, &(colours->at(0).r));

    gl::pushModelView();
    gl::rotate(Vec3f(0, 0, rotation * 180 / M_PI));

    glDrawArrays(drawType, 0, verts->size());

    gl::popModelView();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}


#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Oculus, RendererGl)
#else
    CINDER_APP_BASIC(Oculus, RendererGl)
#endif
