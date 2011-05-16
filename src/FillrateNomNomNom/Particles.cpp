#include "Particles.h"
#include "cinder/Rand.h"

using namespace cinder;
using namespace std;

ParticleSystem::ParticleSystem() : perlin_(Perlin(3)), chaos_(0)
{

}

ParticleSystem::~ParticleSystem()
{
    for (vector<Particle *>::iterator iter = particles_.begin(); iter != particles_.end(); iter ++)
        delete *iter;
    for (vector<Emitter *>::iterator iter = emitters_.begin(); iter != emitters_.end(); iter ++)
        delete *iter;
    for (vector<Attractor *>::iterator iter = attractors_.begin(); iter != attractors_.end(); iter ++)
        delete *iter;
}

void ParticleSystem::Update(float msecs)
{
    Particle *currentParticle = NULL;
    Attractor *currentAttractor = NULL;
    Emitter *currentEmitter = NULL;
    
    // Kill off dead attractors
    vector<Attractor *>::iterator attractorIter = attractors_.begin();
    while (attractorIter != attractors_.end())
    {
        if (!(*attractorIter)->Immortal)
        {
            (*attractorIter)->Life -= msecs;
            if ((*attractorIter)->Life < 0)
            {
                delete (*attractorIter);
                attractorIter = attractors_.erase(attractorIter);
                continue;
            }
        }
        attractorIter ++;
    }

    for (vector<Particle *>::iterator iter = particles_.begin(); iter != particles_.end(); iter ++)
    {
        // Shuffle the particle off the mortal coil if its time is up
        currentParticle = *iter;
        /*
        if (!currentParticle->Immortal)
        {
            currentParticle->Life -= msecs;
            if (currentParticle->Life < 0)
            {   
                iter = particles_.erase(iter);
                delete currentParticle;
                continue;
            }
        }
        */

        for (vector<Attractor *>::iterator attractorIter = attractors_.begin(); attractorIter != attractors_.end(); attractorIter ++)
        {
            currentAttractor = *attractorIter;
            Vec3f dir = (currentAttractor->Position - currentParticle->Position).normalized();
            currentParticle->Velocity += dir * (currentAttractor->Mass / currentParticle->Mass) * 0.01f;
            currentParticle->Velocity *= 0.95f; // friction
        }
        
        // Apply forces
        for (vector<Vec3f>::iterator forceIter = forces_.begin(); forceIter != forces_.end(); forceIter ++)
            currentParticle->Velocity += *forceIter;
            
        currentParticle->Velocity += perlin_.dfBm(currentParticle->Position * 0.01f) * chaos_; // lolrandom        

        // Set final particle position
        currentParticle->Position += currentParticle->Velocity * msecs;
    }

    // Handle emitters
    for (vector<Emitter *>::iterator iter = emitters_.begin(); iter != emitters_.end(); iter ++)
    {
        currentEmitter = *iter;
        currentEmitter->TimeToNextSpawn -= msecs;
        
        if (currentEmitter->TimeToNextSpawn < 0)
        {
            particles_.push_back(CreateParticle(currentEmitter));
            currentEmitter->TimeToNextSpawn += currentEmitter->Frequency;
        }
    }
}

void ParticleSystem::SpawnParticles(Emitter *emitter, unsigned int amount)
{
    for (unsigned int i = 0; i < amount; i ++)
        particles_.push_back(CreateParticle(emitter));
}

Particle *ParticleSystem::CreateParticle(Emitter *emitter)
{
    float x, y, z;
    x = Rand::randFloat(emitter->Position.x - emitter->SpawnPositionVariation.x / 2, emitter->Position.x + emitter->SpawnPositionVariation.x / 2);
    y = Rand::randFloat(emitter->Position.y - emitter->SpawnPositionVariation.y / 2, emitter->Position.y + emitter->SpawnPositionVariation.y / 2);
    z = Rand::randFloat(emitter->Position.z - emitter->SpawnPositionVariation.z / 2, emitter->Position.z + emitter->SpawnPositionVariation.z / 2);

    float vx, vy, vz;
    vx = Rand::randFloat(emitter->MinInitialVelocity.x, emitter->MaxInitialVelocity.x);
    vy = Rand::randFloat(emitter->MinInitialVelocity.y, emitter->MaxInitialVelocity.y);
    vz = Rand::randFloat(emitter->MinInitialVelocity.z, emitter->MaxInitialVelocity.z);

    Particle *particle = new Particle();
    particle->Position = Vec3f(x, y, z);
    particle->Velocity = Vec3f(vx, vy, vz);
    particle->Mass = Rand::randFloat(emitter->MinInitialMass, emitter->MaxInitialMass);
    particle->Size = Rand::randFloat(emitter->MinInitialSize, emitter->MaxInitialSize);
    particle->Immortal = emitter->InitialLife < 0;
    particle->Life = emitter->InitialLife < 0 ? 1 : emitter->InitialLife;
    
    return particle;
}
