#ifndef ARC_PARTICLE_H
#define ARC_PARTICLE_H

#include "site.h"

/*---- γ�Ҿ��� ----*/
class Particle{
 private:
  static int numParticle;
  Site *site;
 public:
  Particle(Site *inSite);
  void  setSite(Site *inSite);
  Site *getSite();
};

#endif
