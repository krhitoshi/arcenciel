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

  unsigned long currentSite;
  unsigned long currentSite2;
  unsigned long nextSite;
  int       pathType;

 public:
  Event(double inRate, 	unsigned long inCurrentSite, unsigned long inNextSite,
	int inPathType);

  Event(double inRate, 	unsigned long inCurrentSite, int inPathType,
	enumEventType inEventType);

  Event(double inRate, 	unsigned long inCurrentSite, unsigned long inCurrentSite2,
	int inPathType, enumEventType inEventType);
  enumEventType getEventType();
  double        getRate();
  unsigned long getCurrentSite();
  unsigned long getCurrentSite2();
  unsigned long getNextSite();
  int           getPathType();
};

#endif
