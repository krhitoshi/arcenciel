/********************************************************************/
/*             Kinetic Monte Carlo Program "ARC" (arc-en-ciel)      */
/*                                                                  */
/*             Copyright (C) 2002-2004 Hitoshi Kurokawa             */
/********************************************************************/
#include <iostream>
#include <string>
using namespace std;

#include "kineticMC.h"

/*************************************************/
/*         �ᥤ��                                */
/*************************************************/
int main(int argc, char *argv[]){
  string silent  = "-s";
  string version = "-v";

  KineticMC arc; /* KMC�׻������� class KineticMC�ǹԤ� */

  /* �������ȥ⡼�ɤ����� */
  if(argc==2 && silent == argv[1]) arc.silentFlagOn();

  /* �ץ����̾�ν��� */
  arc.printProgramName();

  /* �С��������ϤΤߤξ��Ͻ�λ */
  if(argc==2 && version == argv[1]) exit(0);

 /* KMC�׻����� */
  if(!arc.mainProcedure()){
    cout << "Exit!" << endl;
    exit(1);
  }
  
  return 0;
}
