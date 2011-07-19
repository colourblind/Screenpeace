#include "Objects.h"
#include "cinder/gl/gl.h"

using namespace cinder;
using namespace std;

Object::~Object()
{
    for (vector<Object *>::iterator iter = children_.begin(); iter != children_.end(); iter ++)
        delete *iter;
}

Object *ObjectFactory::CreateObject()
{
    Object *result = NULL;
    switch (Rand::randInt(5))
    {
        case 0:
            result = new Transform();
            break;
        case 1:
            result = new Animate();
            break;
        case 2:
            result = new Spawner();
            break;
        default:
            result = new Draw();
            break;
    }
    return result;
}

bool Transform::Update(float msecs)
{
    gl::pushModelView();

    gl::rotate(rotate_);
    gl::scale(scale_);
    gl::translate(translate_);

    Object::Update(msecs);

    gl::popModelView();

    return Object::Update(msecs);
}

bool Animate::Update(float msecs)
{
    scale_ += dScale_ * msecs;
    rotate_ += dRotate_ * msecs;
    translate_ += dTranslate_ * msecs;

    return Transform::Update(msecs);
}

bool Spawner::Update(float msecs)
{
    spawnCountDown_ -= msecs;
    if (spawnCountDown_ < 0)
    {
        children_.push_back(ObjectFactory::CreateObject());
        spawnCountDown_ += spawnTime_;
    }

    return Object::Update(msecs);
}

bool Draw::Update(float msecs)
{
    gl::drawSolidRect(Rectf(-0.5f, -0.5f, 0.5f, 0.5f));
    return Object::Update(msecs);
}
