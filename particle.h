#ifndef ARC_PARTICLE_H
#define ARC_PARTICLE_H

#include "site.h"

/*---- 粒子情報 ----*/
class Particle{
 private:
  static int numParticle;
  unsigned long site;
 public:
  Particle(unsigned long inSite);
  void  setSite(unsigned long inSite);
  unsigned long getSite();
};

/*---- 粒子数を一定に保つための情報 ----*/
class KeepNumParticleConst{
 private:
  unsigned long numParticle;      /* 粒子数 */
  int           particleType;     /* 粒子種(使用しない) */
  int           siteType;         /* サイト種 */
  int           interval;
  vector<unsigned long>  siteVector;

 public:
  KeepNumParticleConst(int inSiteType, unsigned long inNumParticle,
		       int inInterval);
  void addSite(unsigned long site);
  int  getInterval();
  int  getSiteType();
  unsigned long getNumParticle();
  vector<unsigned long> *getSiteVector();

  void clear();
};


#endif
