#include "event.h"

Event::Event(double inRate, Particle *inParticle,
     Site *inCurrentSite, Site *inNextSite, PathType *inPathType){
  rate = inRate;
  particle = inParticle;
  currentSite = inCurrentSite;
  nextSite = inNextSite;
  pathType = inPathType;
}

double Event::getRate(){
  return rate;
}

Particle *Event::getParticle(){
  return particle;
}

Site *Event::getCurrentSite(){
  return currentSite;
}

Site *Event::getNextSite(){
  return nextSite;
}

void Event::occur(){
  currentSite->setState(Site::UNOCCUPY);
  nextSite->setState(Site::OCCUPY);
  particle->setSite(nextSite);
  pathType->occur();
}
