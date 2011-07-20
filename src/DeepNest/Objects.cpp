#include "Objects.h"
#include "Constants.h"
#include "cinder/gl/gl.h"

using namespace cinder;
using namespace std;

Object::~Object()
{
    for (vector<Object *>::iterator iter = children_.begin(); iter != children_.end(); iter ++)
        delete *iter;
}

Object *ObjectFactory::CreateObject(int depth)
{
    Object *result = NULL;
    if (depth < MAX_DEPTH - 1)
    {
        switch (Rand::randInt(5))
        {
            case 0:
                result = new Transform(depth);
                break;
            case 1:
                result = new Animate(depth);
                break;
            case 2:
                result = new Spawner(depth);
                break;
            default:
                result = new Draw();
                break;
        }
    }
    else
        result = new Draw();
    return result;
}

bool Transform::Update(float msecs)
{
    gl::pushModelView();

    gl::rotate(rotate_);
    //gl::scale(scale_);
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
        children_.push_back(new Draw());
        spawnCountDown_ += spawnTime_;
    }

    return Object::Update(msecs);
}

bool Draw::Update(float msecs)
{
    gl::drawCube(Vec3f(0, 0, 0), Vec3f(1, 1, 0.01f));
    return Object::Update(msecs);
}
