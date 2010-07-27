#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/ImageIO.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "Constants.h"
#include "Resources.h"

#include <vector>

using namespace cinder;
using namespace cinder::app;
using namespace std;

#ifdef SCREENSAVER
    #define APP_TYPE    AppScreenSaver
#else
    #define APP_TYPE    AppBasic
#endif

class Circle
{
public:
    Circle() : depth_(-1), visible_(false), position_(Vec2f(0, 0)) { }
    Circle(int depth, bool visible = true) : depth_(depth), visible_(visible) { }
    virtual ~Circle();
 
    void Init();
    void Render() const;
    void Update(float msecs);

protected:
    void AddChildObject(Circle &object) { childObjects_.push_back(&object); }

    int depth_;
    bool visible_;
    vector<Circle *> childObjects_;    

    Vec2f position_;
    float rotation_;
    float scale_;
    float rotationSpeed_;
};

class TimeForBedSaidZebedee : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    Circle circle_;
    Timer timer_;
    gl::Texture texture_;
    CameraOrtho camera_;
};

void TimeForBedSaidZebedee::setup()
{
    circle_.Init();
    
    // Load texture
    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
    format.setMagFilter(GL_LINEAR);
    
    texture_ = gl::Texture(loadImage(loadResource(RES_CIRCLE_IMG)), format);
    texture_.enableAndBind();
    
    gl::enableAlphaBlending();
    gl::disableDepthRead();
    gl::disableDepthWrite();
}

void TimeForBedSaidZebedee::draw()
{
    camera_ = CameraOrtho(0, static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()), 0, -100, 100);

    gl::clear(Color(0, 0, 0));
    gl::setMatrices(camera_);
    gl::translate(Vec2f(static_cast<float>(getWindowWidth()) / 2, static_cast<float>(getWindowHeight()) / 2));
    
    circle_.Render();
}

void TimeForBedSaidZebedee::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();
    
    circle_.Update(msecs);
}

Circle::~Circle()
{
    for (vector<Circle *>::iterator iter = childObjects_.begin(); iter != childObjects_.end(); iter ++)
        delete *iter;
}

void Circle::Init()
{
    if (depth_ > -1)
    {
        float scaleByDepth = (depth_ + 1);
        scale_ = Rand::randFloat(50, 150) / scaleByDepth;
        position_ = Vec2f(Rand::randFloat(-600, 600) / (scaleByDepth * 2), Rand::randFloat(-400, 400) / (scaleByDepth * 2));
    }
    rotation_ = Rand::randFloat(0, 360);
    rotationSpeed_ = Rand::randFloat(-1 * MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
    
    // Create children
    for (int i = 0; i < MAX_DEPTH - depth_; i ++)
    {
        Circle *circle = new Circle(depth_ + 1);
        circle->Init();
        childObjects_.push_back(circle);
    }
}

void Circle::Render() const
{
    gl::pushModelView();

    gl::translate(Vec3f(0, position_.x, position_.y));
    gl::rotate(Vec3f(0, 0, rotation_));

	if (visible_)
	{
        gl::pushModelView();

        gl::scale(Vec3f(scale_, scale_, scale_));
        gl::color(Color(1, 1, 1));
	    // glColor4d(1.0, 1.0, 1.0, (1.0 / depth_ + 1) + 0.5);
        gl::drawSolidRect(Rectf(-0.5f, -0.5f, 0.5f, 0.5f));
        
        gl::popModelView();
    }

    for (vector<Circle *>::const_iterator iter = childObjects_.begin(); iter != childObjects_.end(); iter ++)
        (*iter)->Render();
        
    gl::popModelView();
}

void Circle::Update(float msecs)
{
    rotation_ += rotationSpeed_ * msecs;
    
    for (vector<Circle *>::iterator iter = childObjects_.begin(); iter != childObjects_.end(); iter ++)
        (*iter)->Update(msecs);
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(TimeForBedSaidZebedee, RendererGl)
#else
    CINDER_APP_BASIC(TimeForBedSaidZebedee, RendererGl)
#endif
