#include "event.h"

Event::Event(double inRate, Particle *inParticle,
     Site *inCurrentSite, Site *inNextSite, PathType *inPathType){
  rate        = inRate;
  particle    = inParticle;
  currentSite = inCurrentSite;
  nextSite    = inNextSite;
  pathType    = inPathType;
  eventType   = DIFFUSION;
}

Event::Event(double inRate, Particle *inParticle,
	     Site *inCurrentSite, PathType *inPathType,
	     enumEventType inEventType){
  rate = inRate;
  particle = inParticle;
  currentSite = inCurrentSite;
  pathType = inPathType;
  eventType = inEventType;
}

Event::Event(double inRate, 
	     Particle *inParticle, Particle *inParticle2,
	     Site *inCurrentSite, Site *inCurrentSite2,
	     PathType *inPathType, enumEventType inEventType){
  rate = inRate;
  particle = inParticle;
  particle2 = inParticle2;
  currentSite = inCurrentSite;
  currentSite2 = inCurrentSite2;
  pathType = inPathType;
  eventType = inEventType;
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

void Event::occur(){
  if(eventType==DIFFUSION){
    currentSite->setState(Site::UNOCCUPY);
    nextSite->setState(Site::OCCUPY);
    particle->setSite(nextSite);
    pathType->occur();
  }else if(eventType==DESORPTION){
    currentSite->setState(Site::UNOCCUPY);
    pathType->occur();
  }else if(eventType==ADSORPTION){
    currentSite->setState(Site::UNOCCUPY);
    pathType->occur();
  }
}

Event::enumEventType Event::getEventType(){
  return eventType;
}
