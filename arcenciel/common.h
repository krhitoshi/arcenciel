#ifndef ARC_COMMON_H
#define ARC_COMMON_H

#include "constants.h"

/*---- ��ɸ ----*/
typedef struct position3D{
  float x,y,z;
}pos3D;

/*---- ����ѥ�᡼���� ----*/
typedef struct cellParameters{
  float a,b,c,alpha,beta,gamma;
}cellPara;

/*---- �����Ȥξ��� ----*/
enum siteState {  UNOCCUPY, OCCUPY};

/*---- �����Ⱦ��� ----*/
struct siteInformation{
  unsigned long num;                /* �ֹ� */
  struct siteTypeInformation *type; /* ���� */
  pos3D pos;                        /* ��ɸ */
  enum siteState state;             /* ���� */
  int numNeighbor;                  /* ���ܤ��륵���Ȥο� */
  struct siteInformation **neighbor; /* ���ܤ��륵���ȤΥݥ��󥿤����� */
  struct pathTypeInformation **pathTypeToNeighbor;
};

/*---- �����Ȥμ��� ----*/
struct siteTypeInformation{
  int num;
  char name[NAME_LIMIT];
};

/*---- �ѥ��μ��� ----*/
struct pathTypeInformation{
  int num;
  struct siteTypeInformation *type[2];
  double activEnergy, frequency, rate;
};

/*---- γ�Ҿ��� ----*/
struct particleInformation{
  struct siteInformation *site;
};

/*---- ���٥�Ⱦ��� ----*/
enum eventType{ DIFFUSION, ABSORPTION, DESORPTION };
struct eventInformation{
  enum eventType type;
  double rate;
  struct siteInformation *currentSite;
  struct siteInformation *nextSite;
  struct particleInformation *particle;
};

/*---- ���ܾ��� ----*/
struct basicInformation{
  int outputInterval;
  int numStep;

  cellPara cell;
  unsigned long numSite;
  struct siteInformation *site;

  int siteTypeNum, siteTypeNumMax;
  struct siteTypeInformation *siteType;

  unsigned long numPath;

  int pathTypeNum, pathTypeNumMax;
  struct pathTypeInformation *pathType;

  int numParticle;
  struct particleInformation particle[NUM_PERTICLE];

  unsigned long numEvent, numMaxEvent;
  struct eventInformation *event;
  long double sumRate;

  double temperature, time; /* ���١� ���� */
};

#include "functions.h"

#endif
