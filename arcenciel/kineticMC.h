#ifndef ARC_KINETIC_MC_H
#define ARC_KINETIC_MC_H
#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "particle.h"
#include "site.h"
#include "path.h"
#include "event.h"

class SitePair{
 private:
  unsigned long site1, site2;
 public:
  SitePair(unsigned long inSite1, unsigned long inSite2){
    site1 = inSite1;
    site2 = inSite2;
  }
  unsigned long getSite1(){
    return site1;
  }
  unsigned long getSite2(){
    return site2;
  }
};

class KineticMC{
 private:
  int    fileOutputInterval;         /* ファイル出力インターバル */
  int    displayOutputInterval;      /* 標準出力インターバル */
  int    numStep;                    /* ステップ数 */
  double temperature;                /* 温度 */
  double systemTime, lapSystemTime;  /* 時間 */
  long double sumRate;

  bool timePoisson;
  bool silentFlag;   /* 標準出力の有無 */

  int seedType;      /* 乱数の種 (0ならば時間を種とする)*/
  cellPara cell;     /* セルパラメータ */


  vector<Site>     siteVector;      /* サイト情報 */
  vector<SiteType> siteTypeVector;  /* サイト種情報 */

  vector<PathType> pathTypeVector;  /* パス種情報 */

  vector<Particle> particleVector;  /* 粒子情報   */

  vector<Event>    eventVector;     /* イベント情報 */

  vector<unsigned long>    adsorptionSiteVector;
  vector<unsigned long>    desorptionSiteVector;
  vector<SitePair> dissosiativeAdsorptionSiteVector;
  vector<SitePair> recombinativeDesorptionSiteVector;

  /* 入力ファイル名 */
  string siteFileName;
  string pathFileName;
  string rateFileName;
  string particleFileName;

 public:
  KineticMC();
  bool mainProcedure();

  void printProgramName();
  void silentFlagOn();
  void silentFlagOff();

 private:
  void mainLoop();

  void initialize();
  void updateSystemTime();
  void clear();
 
  void printSiteInformation();
  void printRateInformation();
  void printInputData();
  void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time);
  void printOccurrence(int step, ostream &stream);

  void createPathToExternalPhase();


  void loadInputFile();
  void loadSite();
  void loadParticle();
  void loadPath();
  void loadRate();

  void loadCellParameters( const char *line);
  void loadCoordination( const char *line);
  void loadPair( const char *line);

  void countEvent();
  void eventOccur(vector<Event>::size_type index);
  
  void checkTopSurface();
  void checkButtomSurface();

  void   addSiteNeighbor(Site *site,Site *neighbor);
  double getRandomNumber();
  int findSiteType(char *name);
  int findSiteTypeNoAppend(char *name);
  int addSiteType(char *name);
  int findPathType(int type1, int type2);

  int getRealNumNeighbor(int site);
};

#endif
