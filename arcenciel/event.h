#ifndef ARC_EVENT_H
#define ARC_EVENT_H

/*---- ���٥�Ⱦ��� ----*/
enum eventType{ DIFFUSION, ABSORPTION, DESORPTION };
struct eventInformation{
  enum eventType type;
  double rate;
  struct siteInformation *currentSite;
  struct siteInformation *nextSite;
  struct particleInformation *particle;
};

#endif
