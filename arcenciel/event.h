#ifndef ARC_EVENT_H
#define ARC_EVENT_H

/*---- イベント情報 ----*/
#include "site.h"

enum eventType{ DIFFUSION, ABSORPTION, DESORPTION };
struct eventInformation{
  enum eventType type;
  double rate;
  Site *currentSite;
  Site *nextSite;
  struct particleInformation *particle;
};

#endif
