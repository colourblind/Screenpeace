#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/ImageIO.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Xml.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
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

struct Glyph
{
    Glyph() { page = -1; }
    Glyph(XmlTree element)
    {
        x = element.getAttributeValue<int>("x");
        y = element.getAttributeValue<int>("y");
        width = element.getAttributeValue<int>("width");
        height = element.getAttributeValue<int>("height");
        offsetX = element.getAttributeValue<int>("xoffset");
        offsetY = element.getAttributeValue<int>("yoffset");
        advanceX = element.getAttributeValue<int>("xadvance");
        page = element.getAttributeValue<int>("page");
    }

    int x, y;
    int width, height;
    int offsetX, offsetY;
    int advanceX;
    int page;
};

struct Snippet
{
    Snippet() : 
        text("foo"), 
        position(Vec3f(Rand::randFloat(-200, 200), Rand::randFloat(-200, 200), Rand::randFloat(-200, 200))),
        // position(Vec3f(0, 0, 0)),
        up(Vec3f(0, -1, 0)),
        forward(Vec3f(Rand::randFloat(-1, 1), 0, Rand::randFloat(-1, 1))),
        totalSize(Vec2f(0, 0))
    {
        forward.normalize();
    }

    std::string text;
    Vec3f position;
    Vec3f up;
    Vec3f forward;
    gl::VboMesh mesh[2];
    Vec2f totalSize;
};

class Hypertext : public APP_TYPE
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    void GenerateMesh(int index);

    Timer timer_;
    Snippet snippets_[NUM_SNIPPETS];
    int nextSnippet_;
    int prevSnippet_;
    float totalTravelTime_;
    float currentTravelTime_;

    vector<gl::Texture> fontPages_;
    Glyph glyphs_[256];
};

void Hypertext::setup()
{
    Rand::randomize();

    nextSnippet_ = Rand::randInt(NUM_SNIPPETS);
    prevSnippet_ = nextSnippet_;
    totalTravelTime_ = 2000;
    currentTravelTime_ = 0;

    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
    format.setMagFilter(GL_LINEAR);
    
    fontPages_.push_back(gl::Texture(loadImage(loadResource(RES_FONT_IMG1)), format));
    fontPages_.push_back(gl::Texture(loadImage(loadResource(RES_FONT_IMG2)), format));

    XmlTree doc(loadResource(RES_FONT_DEFINITION));
    XmlTree chars = doc.getChild("font/chars");
    for(XmlTree::Iter child = chars.begin(); child != chars.end(); child ++)
    {
        int index = child->getAttributeValue<int>("id");
        glyphs_[index] = Glyph(*child);
    }

    for (int i = 0; i < NUM_SNIPPETS; i ++)
        GenerateMesh(i);

    gl::enableAlphaBlending();
    //gl::disableDepthRead();
    //gl::disableDepthWrite();
}

void Hypertext::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    currentTravelTime_ += msecs;
    if (currentTravelTime_ > totalTravelTime_)
    {
        if (prevSnippet_ == nextSnippet_) // finished pausing, move on
        {
            prevSnippet_ = nextSnippet_;
            nextSnippet_ = Rand::randInt(NUM_SNIPPETS);
        }
        else
        {
            prevSnippet_ = nextSnippet_;
        }
        currentTravelTime_ = 0;
    }
}

void Hypertext::draw()
{
    float windowWidth = static_cast<float>(getWindowWidth());
    float windowHeight = static_cast<float>(getWindowHeight());

    gl::clear();

    CameraPersp camera = CameraPersp(windowWidth, windowHeight, 60, 1, 1000);

    float lerp = currentTravelTime_ / totalTravelTime_;

    Vec3f camUp = (snippets_[nextSnippet_].up - snippets_[prevSnippet_].up) * lerp + snippets_[prevSnippet_].up;
    Vec3f camTarget = (snippets_[nextSnippet_].position - snippets_[prevSnippet_].position) * lerp + snippets_[prevSnippet_].position;
    Vec3f eyePos = ((snippets_[nextSnippet_].position + snippets_[nextSnippet_].forward * -RANGE)
            - (snippets_[prevSnippet_].position + snippets_[prevSnippet_].forward * -RANGE)) * lerp
            + (snippets_[prevSnippet_].position + snippets_[prevSnippet_].forward * -RANGE);

    camera.lookAt(eyePos, camTarget, camUp);
    gl::setMatrices(camera);

    for (int i = 0; i < NUM_SNIPPETS; i ++)
    {
        Vec3f up = snippets_[i].up;
        Vec3f forward = snippets_[i].forward;
        Vec3f rot = Vec3f(0, atan2f(forward.x, forward.z), 0) * 180 / M_PI;

        gl::pushModelView();
        gl::translate(snippets_[i].position);
        gl::rotate(rot);

        if (nextSnippet_ == i)
            gl::color(Color(1, 0, 0));
        else
            gl::color(Color(1, 1, 1));

        fontPages_[0].enableAndBind();
        gl::drawArrays(snippets_[i].mesh[0], 0, snippets_[i].mesh[0].getNumVertices());
        fontPages_[1].enableAndBind();
        gl::drawArrays(snippets_[i].mesh[1], 0, snippets_[i].mesh[1].getNumVertices());

        gl::popModelView();
    }
}

void Hypertext::GenerateMesh(int index)
{
    int currentX = 0;
    int currentY = 0;

    vector<vector<Vec3f>> positions;
    vector<vector<uint32_t>> indices;
	vector<vector<Vec2f>> texCoords;

    positions.resize(2);
    indices.resize(2);
    texCoords.resize(2);

    float texScale = 1.0f / 256;

    for (int i = 0; i < snippets_[index].text.length(); i ++)
    {
        int glyphIndex = static_cast<int>(snippets_[index].text[i]);
        Glyph current = glyphs_[glyphIndex];
        int j = current.page;

        int x = current.offsetX + currentX;
        int y = current.offsetY + currentY;

        positions[j].push_back(Vec3f(x, y, 0));
        positions[j].push_back(Vec3f(x, y + current.height, 0));
        positions[j].push_back(Vec3f(x + current.width, y + current.height, 0));
        positions[j].push_back(Vec3f(x + current.width, y, 0));

        indices[j].push_back(i * 4 + 0);
        indices[j].push_back(i * 4 + 1);
        indices[j].push_back(i * 4 + 2);
        indices[j].push_back(i * 4 + 3);

        texCoords[j].push_back(Vec2f(current.x, current.y) * texScale);
        texCoords[j].push_back(Vec2f(current.x, current.y + current.height) * texScale);
        texCoords[j].push_back(Vec2f(current.x + current.width, current.y + current.height) * texScale);
        texCoords[j].push_back(Vec2f(current.x + current.width, current.y) * texScale);

        currentX += current.advanceX;
    }

    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setStaticPositions();
    layout.setStaticTexCoords2d();
    
    snippets_[index].mesh[0] = gl::VboMesh(positions[0].size(), indices[0].size(), layout, GL_QUADS); 
    snippets_[index].mesh[0].bufferPositions(positions[0]);
    snippets_[index].mesh[0].bufferIndices(indices[0]);
    snippets_[index].mesh[0].bufferTexCoords2d(0, texCoords[0]);

    snippets_[index].mesh[1] = gl::VboMesh(positions[1].size(), indices[1].size(), layout, GL_QUADS);
    snippets_[index].mesh[1].bufferPositions(positions[1]);
    snippets_[index].mesh[1].bufferIndices(indices[1]);
    snippets_[index].mesh[1].bufferTexCoords2d(0, texCoords[1]);
}

#ifdef SCREENSAVER
    CINDER_APP_SCREENSAVER(Hypertext, RendererGl)
#else
    CINDER_APP_BASIC(Hypertext, RendererGl)
#endif
