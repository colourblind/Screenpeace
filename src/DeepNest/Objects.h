#ifndef DEEPNEST_OBJECTS_H
#define DEEPNEST_OBJECTS_H

#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include <vector>

class Object
{
public:
    Object() : life_(cinder::Rand::randFloat(5000, 20000)) { }
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
    static Object *CreateObject(int depth);
};

class Draw : public Object
{
public:
    virtual bool Update(float msecs);
};

class Spawner : public Object
{
public:
    Spawner(int depth) : spawnTime_(cinder::Rand::randFloat(500, 2000)), spawnCountDown_(0) { }
    virtual bool Update(float msecs);

protected:
    float spawnCountDown_;
    float spawnTime_;
};

class Transform : public Object
{
public:
    Transform(int depth) : 
        scale_(cinder::Rand::randFloat(0.75, 1.5), cinder::Rand::randFloat(0.75, 1.5), cinder::Rand::randFloat(0.75, 1.5)), 
        translate_(cinder::Rand::randFloat(-6, 6), cinder::Rand::randFloat(-6, 6), cinder::Rand::randFloat(-6, 6)),
        rotate_(0, cinder::Rand::randFloat(0, 2 * M_PI), 0)
    { 
        for (int i = 0; i < cinder::Rand::randInt(10, 20); i ++)
            children_.push_back(ObjectFactory::CreateObject(depth + 1));        
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
    Animate(int depth)  : 
        Transform(depth),
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
