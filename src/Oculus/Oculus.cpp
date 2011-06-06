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

class Segment
{
public:
    Segment() : 
        centre_(Vec3f(0, 0, 0)),
        radius_(Rand::randFloat(1, 10)),
        startAngle_(Rand::randFloat(0, M_PI * 2)),
        angle_(Rand::randFloat(MIN_SEGMENT_LENGTH, MAX_SEGMENT_LENGTH)),
        startColour_(ColorA(Rand::randFloat(0, 0.5f), Rand::randFloat(0, 0.5f), 1), Rand::randFloat(0.5f, 1)),
        endColour_(ColorA(Rand::randFloat(0, 0.5f), Rand::randFloat(0, 0.5f), 1), Rand::randFloat(0.5f, 1)),
        rotation_(0),
        rotationSpeed_(Rand::randFloat(-MAX_ROTATION_SPEED, MAX_ROTATION_SPEED)),
        renderType_(Rand::randInt(2))
    {
        thickness_ = Rand::randFloat(0.1f, radius_ / 8);
        slices_ = static_cast<int>(angle_ * CURVE_FIDELITY);
    }
    
    void SetRadius(float radius) { radius_ = radius; }
    void SetStartAngle(float startAngle) { startAngle_ = startAngle; }
    void SetAngle(float angle) { angle_ = angle; }
    void Render();
    void Update(float msecs)
    {
        rotation_ += rotationSpeed_ * msecs;
        if (rotation_ < 0)
            rotation_ += M_PI * 2;
        if (rotation_ > M_PI * 2)
            rotation_ -= M_PI * 2;
    }

protected:
    Vec3f centre_;
    float radius_;
    float thickness_;
    float startAngle_;
    float angle_;
    ColorA startColour_;
    ColorA endColour_;
    int slices_;
    float rotation_;
    float rotationSpeed_;
    int renderType_;
};

class Oculus : public APP_TYPE
{
public:
    virtual void setup();
    virtual void update();
    virtual void draw();
    
private:
    Segment segments_[NUM_SEGMENTS];
    Timer timer_;
};

void Segment::Render()
{
    vector<Vec3f> verts;
    vector<ColorA> colours;

    float halfThick = thickness_ / 2;
    float angleStep = angle_ / slices_;

    if (renderType_ == 0)
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
    else if (renderType_ == 1)
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
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &verts[0].x);
	glColorPointer(4, GL_FLOAT, 0, &colours[0].r);

    gl::pushModelView();
    gl::rotate(Vec3f(0, 0, rotation_ * 180 / M_PI));

    GLenum drawType = renderType_ == 0 ? GL_QUADS : GL_LINE_LOOP;
	glDrawArrays(drawType, 0, verts.size());

    gl::popModelView();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void Oculus::setup()
{
    Rand::randomize();

    gl::enableAlphaBlending();
}

void Oculus::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    for (int i = 0; i < NUM_SEGMENTS; i ++)
        segments_[i].Update(msecs);
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

    for (int i = 0; i < NUM_SEGMENTS; i ++)
        segments_[i].Render();
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Oculus, RendererGl)
#else
    CINDER_APP_BASIC(Oculus, RendererGl)
#endif
