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
  string silent  = "-s";
  string version = "-v";

  KineticMC arc;
  if(argc==2 && silent == argv[1]) arc.silentFlagOn();

  arc.printProgramName();
  if(argc==2 && version == argv[1]) exit(0);

  if(!arc.mainProcedure()){
    cout << "Exit!" << endl;
    exit(1);
  }
  
  return 0;
}
