#include "particle.h"

int Particle::numParticle;

Particle::Particle(Site *inSite){
  site = inSite;
}

void Particle::setSite(Site *inSite){
  site = inSite;
}

Site *Particle::getSite(){
  return site;
}
