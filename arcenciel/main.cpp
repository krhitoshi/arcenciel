/********************************************************************/
/*             Kinetic Monte Carlo Program "ARC" (arc-en-ciel)      */
/*                                                                  */
/*             Copyright (C) 2002-2003 Hitoshi Kurokawa             */
/********************************************************************/
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "kineticMC.h"

/*************************************************/
/*         еседеє                                */
/*************************************************/
int main(int argc, char *argv[]){
  string option = "-v";

  KineticMC arc;
  arc.printProgramName();
  if(argc==2 && option == argv[1]) exit(0);

  arc.mainProcedure();
  
  return 0;
}
