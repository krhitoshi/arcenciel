#ifndef ARC_EVENT_H
#define ARC_EVENT_H

/*---- ���٥�Ⱦ��� ----*/
#include "site.h"
#include "path.h"
#include "particle.h"

class Event{
 public:
  enum enumEventType{ 
    DIFFUSION, ADSORPTION, DESORPTION,
    DISSOCIATIVE_ADSORPTION, RECOMBINATIVE_DESORPTION
  };

 private:
  enumEventType eventType;
  double rate;

  Particle *particle;
  Particle *particle2;
  Site     *currentSite;
  Site     *currentSite2;
  Site     *nextSite;
  int       pathType;


 public:
  Event(double inRate, Particle *inParticle,
	Site *inCurrentSite, Site *inNextSite, int inPathType);

  Event(double inRate, Particle *inParticle,
	Site *inCurrentSite, int inPathType,
	enumEventType inEventType);

  Event(double inRate, Particle *inParticle, Particle *inParticle2,
	Site *inCurrentSite, Site *inCurrentSite2,
	int inPathType, enumEventType inEventType);
  enumEventType getEventType();
  double    getRate();
  void      occur();
  Particle *getParticle();
  Site     *getCurrentSite();
  Site     *getCurrentSite2();
  int       getPathType();
};

#endif
