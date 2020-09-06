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

KeepNumParticleConst::KeepNumParticleConst( int inSiteType,
		    unsigned long inNumParticle, int inInterval){
  siteType    = inSiteType;        
  numParticle = inNumParticle;     
  interval    = inInterval;
}

void KeepNumParticleConst::clear(){
  siteVector.clear();
}

void KeepNumParticleConst::addSite(unsigned long site){
  siteVector.push_back(site);
}

int KeepNumParticleConst::getInterval(){
  return interval;
}

int KeepNumParticleConst::getSiteType(){
  return siteType;
}

unsigned long KeepNumParticleConst::getNumParticle(){
  return numParticle;
}

vector<unsigned long> *KeepNumParticleConst::getSiteVector(){
  return &siteVector;
}

