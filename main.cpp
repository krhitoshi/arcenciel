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
/*         メイン                                */
/*************************************************/
int main(int argc, char *argv[]){
  string silent  = "-s";
  string version = "-v";

  KineticMC arc; /* KMC計算は全て class KineticMCで行う */

  /* サイレントモードの設定 */
  if(argc==2 && silent == argv[1]) arc.silentFlagOn();

  /* プログラム名の出力 */
  arc.printProgramName();

  /* バージョン出力のみの場合は終了 */
  if(argc==2 && version == argv[1]) exit(0);

 /* KMC計算開始 */
  if(!arc.mainProcedure()){
    cout << "Exit!" << endl;
    exit(1);
  }
  
  return 0;
}
