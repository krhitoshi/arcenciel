#ifndef ARC_EVENT_H
#define ARC_EVENT_H

/*---- ���٥�Ⱦ��� ----*/
#include "site.h"
#include "particle.h"

enum eventType{ DIFFUSION, ABSORPTION, DESORPTION };
struct eventInformation{
  enum eventType type;
  double rate;
  Site *currentSite;
  Site *nextSite;
  Particle *particle;
};

#endif
