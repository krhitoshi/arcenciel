/*****************************************************************************/
/*             Kinetic Monte Carlo Program "ARC" (arc-en-ciel)               */
/*                                                                           */
/*             Copyright (C) 2002-2003 Hitoshi Kurokawa                      */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "kineticMC.h"

struct basicInformation kmc;

/*************************************************/
/*         еседеє                                */
/*************************************************/
int main(){
  int i;
  unsigned long random;

  KineticMC arc;
  arc.loadInputFile();
  arc.printInputData();

  initialize();
  loadInput();
  loadSite();
  loadRate();
  loadPath();

  kmc.numParticle = NUM_PERTICLE;

  printInputData();

  for(i=0;i<kmc.numParticle;i++){
    random = (unsigned long)
      ((double)rand()/(double)RAND_MAX*kmc.numSite);
    if(kmc.site[random].state==UNOCCUPY){
      kmc.site[random].state=OCCUPY;
      kmc.particle[i].site = &kmc.site[random];
    }
  }
 
  mainLoop();

  return 0;
}
