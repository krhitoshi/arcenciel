#ifndef ARC_PARTICLE_H
#define ARC_PARTICLE_H

#include "site.h"

/*---- γ�Ҿ��� ----*/
class Particle{
 private:
  static int numParticle;
  unsigned long site;
 public:
  Particle(unsigned long inSite);
  void  setSite(unsigned long inSite);
  unsigned long getSite();
};

#endif
