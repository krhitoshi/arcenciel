#ifndef ARC_COMMON_H
#define ARC_COMMON_H

#include "constants.h"

#define MAX_NUM_PERTICLE 10000

/*---- 座標 ----*/
typedef struct position3D{
  float x,y,z;
}pos3D;

/*---- セルパラメーター ----*/
typedef struct cellParameters{
  float a,b,c,alpha,beta,gamma;
}cellPara;


#endif
