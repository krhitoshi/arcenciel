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

/*************************************************/
/*         еседеє                                */
/*************************************************/
int main(){

  KineticMC arc;
  arc.printProgramName();
  arc.loadInputFile();
  arc.printInputData();
  arc.loadSite();
  arc.loadRate();
  arc.loadPath();

  arc.putParticles();
  arc.mainLoop();
  
  return 0;
}
