#include "event.h"

Event::Event(double inRate, Particle *inParticle,
     unsigned long inCurrentSite, unsigned long inNextSite, int inPathType){
  rate        = inRate;
  particle    = inParticle;
  currentSite = inCurrentSite;
  nextSite    = inNextSite;
  pathType    = inPathType;
  eventType   = DIFFUSION;
}

Event::Event(double inRate, Particle *inParticle,
	     unsigned long inCurrentSite, int inPathType,
	     enumEventType inEventType){
  rate = inRate;
  particle = inParticle;
  currentSite = inCurrentSite;
  pathType = inPathType;
  eventType = inEventType;
}

Event::Event(double inRate, 
	     Particle *inParticle, Particle *inParticle2,
	     unsigned long inCurrentSite, unsigned long inCurrentSite2,
	     int inPathType, enumEventType inEventType){
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

int Event::getPathType(){
  return pathType;
}

Particle *Event::getParticle(){
  return particle;
}

unsigned long Event::getCurrentSite(){
  return currentSite;
}

unsigned long Event::getCurrentSite2(){
  return currentSite2;
}

void Event::occur(){
//    if(eventType==DIFFUSION){
//      currentSite->setState(Site::UNOCCUPY);
//      nextSite->setState(Site::OCCUPY);
//      particle->setSite(nextSite);


//    }else if(eventType==DESORPTION){
//      currentSite->setState(Site::UNOCCUPY);


//    }else if(eventType==ADSORPTION){
//
//      /* 粒子を生成する際にOCCUPYにしているのでここでは設定不要 */


//    }else if(eventType==DISSOCIATIVE_ADSORPTION){
//      /* 粒子を生成する際にOCCUPYにしているのでここでは設定不要 */


//    }else if(eventType==RECOMBINATIVE_DESORPTION){
//      currentSite->setState(Site::UNOCCUPY);
//      currentSite2->setState(Site::UNOCCUPY);
//    }
}

Event::enumEventType Event::getEventType(){
  return eventType;
}

unsigned long Event::getNextSite(){
  return nextSite;
}
