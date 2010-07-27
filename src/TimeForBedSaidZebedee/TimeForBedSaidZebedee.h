

class Circle : public YaleLite::Engine::Object
{
public:
    Circle(int depth, bool visible = true);
    virtual ~Circle();
    
    virtual void Init();
    virtual void Render() const;
    virtual void Update(double msecs);
    
    void AddChildObject(Object &object) { childObjects_.push_back(&object); }

protected:
    int depth_;
    std::list<Object *> childObjects_;
    
    GLuint texture_;

    bool visible_;
    double rotationSpeed_;
    
    Yale::Maths::Vertex3 position_;
    double rotation_;
    double scale_;
};
