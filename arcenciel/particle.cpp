#include "particle.h"

int Particle::numParticle;

Particle::Particle(unsigned long inSite){
  site = inSite;
}

void Particle::setSite(unsigned long inSite){
  site = inSite;
}

unsigned long Particle::getSite(){
  return site;
}
