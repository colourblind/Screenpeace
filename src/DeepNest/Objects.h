#ifndef DEEPNEST_OBJECTS_H
#define DEEPNEST_OBJECTS_H

#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include <vector>

class Object
{
public:
    Object() : life_(cinder::Rand::randFloat(1000, 10000)) { }
    virtual ~Object();
    virtual bool Update(float msecs)
    {
        std::vector<Object *>::iterator iter = children_.begin();
        while (iter != children_.end())
        {
            if ((*iter)->Update(msecs))
                iter = children_.erase(iter);
            else
                iter ++;
        }

        life_ -= msecs;
        return life_ < 0;
    }

protected:
    std::vector<Object *> children_;
    float life_;
};

class ObjectFactory
{
public:
    static Object *CreateObject();
};

class Draw : public Object
{
public:
    virtual bool Update(float msecs);
};

class Spawner : public Object
{
public:
    Spawner() : spawnTime_(cinder::Rand::randFloat(250, 2000)), spawnCountDown_(0) { }
    virtual bool Update(float msecs);

protected:
    float spawnCountDown_;
    float spawnTime_;
};

class Transform : public Object
{
public:
    Transform() : 
        scale_(cinder::Rand::randFloat(0.5, 2), cinder::Rand::randFloat(0.5, 2), cinder::Rand::randFloat(0.5, 2)), 
        translate_(cinder::Rand::randFloat(-4, 4), cinder::Rand::randFloat(-4, 4), cinder::Rand::randFloat(-4, 4)),
        rotate_(0, cinder::Rand::randFloat(0, 2 * M_PI), 0)
    { 
        for (int i = 0; i < cinder::Rand::randInt(5, 10); i ++)
            children_.push_back(new Draw());        
    }
    
    virtual bool Update(float msecs);

protected:
    cinder::Vec3f scale_;
    cinder::Vec3f translate_;
    cinder::Vec3f rotate_;
};

class Animate : public Transform
{
public:
    Animate()  : 
        dScale_(cinder::Rand::randFloat(-0.001f, 0.001f), cinder::Rand::randFloat(-0.001f, 0.001f), cinder::Rand::randFloat(-0.001f, 0.001f)), 
        dTranslate_(cinder::Rand::randFloat(-0.0001f, 0.0001f), cinder::Rand::randFloat(-0.0001f, 0.0001f), cinder::Rand::randFloat(-0.0001f, 0.0001f)),
        dRotate_(0, cinder::Rand::randFloat(-0.0001f, 0.0001f), 0)
    { }
    virtual bool Update(float msecs);

protected:
    cinder::Vec3f dScale_;
    cinder::Vec3f dTranslate_;
    cinder::Vec3f dRotate_;
};

#endif // DEEPNEST_OBJECTS_H
