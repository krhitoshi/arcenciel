#ifndef ARC_EVENT_H
#define ARC_EVENT_H

/*---- イベント情報 ----*/
#include "site.h"
#include "path.h"
#include "particle.h"

class Event{
 public:
  enum enumEventType{ DIFFUSION, ADSORPTION, DESORPTION };

 private:
  enumEventType eventType;
  double rate;
  Particle *particle;
  Site *currentSite;
  Site *nextSite;
  PathType *pathType;

 public:
  Event(double inRate, Particle *inParticle,
	Site *inCurrentSite, Site *inNextSite, PathType *inPathType);
  Event(double inRate, Particle *inParticle,
	Site *inCurrentSite, PathType *inPathType,
	enumEventType inEventType);
  enumEventType getEventType();
  double    getRate();
  void      occur();
  Particle *getParticle();
  Site     *getCurrentSite();
};

#endif
