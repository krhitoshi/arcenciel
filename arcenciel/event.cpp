#include "event.h"

Event::Event(double inRate, 
     unsigned long inCurrentSite, unsigned long inNextSite, int inPathType){
  rate        = inRate;
  currentSite = inCurrentSite;
  nextSite    = inNextSite;
  pathType    = inPathType;
  eventType   = DIFFUSION;
}

Event::Event(double inRate, 
	     unsigned long inCurrentSite, int inPathType,
	     enumEventType inEventType){
  rate = inRate;
  currentSite = inCurrentSite;
  pathType = inPathType;
  eventType = inEventType;
}

Event::Event(double inRate, 
	     unsigned long inCurrentSite, unsigned long inCurrentSite2,
	     int inPathType, enumEventType inEventType){
  rate = inRate;
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

unsigned long Event::getCurrentSite(){
  return currentSite;
}

unsigned long Event::getCurrentSite2(){
  return currentSite2;
}

Event::enumEventType Event::getEventType(){
  return eventType;
}

unsigned long Event::getNextSite(){
  return nextSite;
}
