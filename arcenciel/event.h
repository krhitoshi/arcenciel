#ifndef ARC_EVENT_H
#define ARC_EVENT_H

/*---- イベント情報 ----*/
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
  unsigned long currentSite;
  unsigned long currentSite2;
  unsigned long nextSite;
  int       pathType;


 public:
  Event(double inRate, Particle *inParticle,
	unsigned long inCurrentSite, unsigned long inNextSite,
	int inPathType);

  Event(double inRate, Particle *inParticle,
	unsigned long inCurrentSite, int inPathType,
	enumEventType inEventType);

  Event(double inRate, Particle *inParticle, Particle *inParticle2,
	unsigned long inCurrentSite, unsigned long inCurrentSite2,
	int inPathType, enumEventType inEventType);
  enumEventType getEventType();
  double        getRate();
  void          occur();
  Particle     *getParticle();
  unsigned long getCurrentSite();
  unsigned long getCurrentSite2();
  unsigned long getNextSite();
  int           getPathType();
};

#endif
