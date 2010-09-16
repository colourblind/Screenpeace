#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>
#include "cinder/Perlin.h"
#include "cinder/Vector.h"

struct Particle
{
    cinder::Vec3f Position;
    cinder::Vec3f Velocity;
    float Mass;
    float Size;
    bool Immortal;
    float Life;
};

struct Emitter
{
    cinder::Vec3f Position;
    cinder::Vec3f SpawnPositionVariation;
    cinder::Vec3f MinInitialVelocity;
    cinder::Vec3f MaxInitialVelocity;
    float MinInitialMass;
    float MaxInitialMass;
    float MinInitialSize;
    float MaxInitialSize;
    float InitialLife;
    float Frequency;
    float TimeToNextSpawn;
};

struct Attractor
{
    cinder::Vec3f Position;
    float Mass;
    bool Immortal;
    float Life;
};

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();
    
    void Update(float msecs);
    
    void AddEmitter(Emitter *emitter) { emitters_.push_back(emitter); }
    void AddAttractor(Attractor *attractor) { attractors_.push_back(attractor); }
    void AddForce(cinder::Vec3f force) { forces_.push_back(force); }
    
    void SpawnParticles(Emitter *emitter, unsigned int amount);
    
    unsigned int GetParticleCount() { return particles_.size(); }
    std::vector<Particle *> *GetParticles() { return &particles_; }
    unsigned int GetAttractorCount() { return attractors_.size(); }
    std::vector<Attractor *> *GetAttractors() { return &attractors_; }

    void SetChaos(float chaos) { chaos_ = chaos; }

private:
    Particle *CreateParticle(Emitter *emitter);

    std::vector<Particle *> particles_;
    std::vector<Emitter *> emitters_;
    std::vector<Attractor *> attractors_;
    std::vector<cinder::Vec3f> forces_;
    cinder::Perlin perlin_;
    float chaos_;
};

#endif // PARTICLES_H
