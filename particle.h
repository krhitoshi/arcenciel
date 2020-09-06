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

/*---- γ�ҿ��������ݤĤ���ξ��� ----*/
class KeepNumParticleConst{
 private:
  unsigned long numParticle;      /* γ�ҿ� */
  int           particleType;     /* γ�Ҽ�(���Ѥ��ʤ�) */
  int           siteType;         /* �����ȼ� */
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
