#ifndef ARC_EVENT_H
#define ARC_EVENT_H

/*---- イベント情報 ----*/
#include "site.h"
#include "particle.h"

class Event{
 public:
  enum enumEventType{ DIFFUSION, ABSORPTION, DESORPTION };
 private:
  enumEventType type;
  double rate;
  Particle *particle;
  Site *currentSite;
  Site *nextSite;
 public:
  Event(double inRate, Particle *inParticle,
	Site *inCurrentSite, Site *inNextSite);
  double    getRate();
  void      occur();
  Particle *getParticle();
  Site     *getCurrentSite();
  Site     *getNextSite();
};

#endif
