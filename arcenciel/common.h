#ifndef ARC_COMMON_H
#define ARC_COMMON_H

#include "constants.h"

/*---- 座標 ----*/
typedef struct position3D{
  float x,y,z;
}pos3D;

/*---- セルパラメーター ----*/
typedef struct cellParameters{
  float a,b,c,alpha,beta,gamma;
}cellPara;

/*---- サイトの状態 ----*/
enum siteState {  UNOCCUPY, OCCUPY};

/*---- サイト情報 ----*/
struct siteInformation{
  unsigned long num;                /* 番号 */
  struct siteTypeInformation *type; /* 種類 */
  pos3D pos;                        /* 座標 */
  enum siteState state;             /* 状態 */
  int numNeighbor;                  /* 隣接するサイトの数 */
  struct siteInformation **neighbor; /* 隣接するサイトのポインタの配列 */
  struct pathTypeInformation **pathTypeToNeighbor;
};

/*---- サイトの種類 ----*/
struct siteTypeInformation{
  int num;
  char name[NAME_LIMIT];
};

/*---- パスの種類 ----*/
struct pathTypeInformation{
  int num;
  struct siteTypeInformation *type[2];
  double activEnergy, frequency, rate;
};

/*---- 粒子情報 ----*/
struct particleInformation{
  struct siteInformation *site;
};

/*---- イベント情報 ----*/
enum eventType{ DIFFUSION, ABSORPTION, DESORPTION };
struct eventInformation{
  enum eventType type;
  double rate;
  struct siteInformation *currentSite;
  struct siteInformation *nextSite;
  struct particleInformation *particle;
};

/*---- 基本情報 ----*/
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

  double temperature, time; /* 温度、 時間 */
};

#include "functions.h"

#endif
