#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"

#include <string>

class Swirl : public Colourblind::ScreenSaver
{
public:
    Swirl(HWND window) : Colourblind::ScreenSaver(window) { };

    virtual void Init();
};

class SwirlBox : public YaleLite::Engine::Object
{
public:
    SwirlBox();
	virtual void Render() const;
	virtual void Update(double msecs);
	
	void SetTexture(std::string textureName);
	
protected:
    GLuint texture_;
    Yale::Maths::Vector3 rotation_;
    Yale::Maths::Vector3 rotationSpeed_;
    Yale::Maths::Vector3 colour_;
};

class MotionBlurHack : public YaleLite::Engine::Object
{
public:
    virtual void Render() const;
    virtual void Update(double msecs) { }
};

