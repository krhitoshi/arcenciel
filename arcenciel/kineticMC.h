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
  Site *site1, *site2;
 public:
  SitePair(Site *inSite1, Site *inSite2){
    site1 = inSite1;
    site2 = inSite2;
  }
  Site *getSite1(){
    return site1;
  }
  Site *getSite2(){
    return site2;
  }
};

class KineticMC{
 private:
  ofstream stdoutFile;
  int    fileOutputInterval;
  int    displayOutputInterval;
  int    numStep;
  double temperature; /* 温度 */
  double systemTime, lapSystemTime;  /* 時間 */

  bool timePoisson;
  bool silentFlag;   /* 標準出力の有無 */

  int seedType;
  cellPara cell;

  vector<Site>     siteVector;
  vector<Site*>    adsorptionSiteVector;
  vector<Site*>    desorptionSiteVector;
  vector<SitePair> dissosiativeAdsorptionSiteVector;
  vector<SitePair> recombinativeDesorptionSiteVector;

  vector<SiteType> siteTypeVector;

  unsigned long numPath;
  vector<PathType> pathTypeVector;

  int numParticle;
  vector<Particle> particleVector;

  vector<Event>    eventVector;
  long double sumRate;

  /* ファイル名 */
  string siteFileName;
  string pathFileName;
  string rateFileName;


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
  void clearVectors();
 
  void printSiteInformation();
  void printRateInformation();
  void printInputData();
  void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time);
  void printOccurrence(int step, ostream &stream);

  void createPathToExternalPhase();


  void loadInputFile();
  void loadSite();
  void loadPath();
  void loadRate();
  void loadSiteType();

  void loadCellParameters( const char *line);
  void loadCoordination( const char *line);
  void loadPair( const char *line);

  void putParticles();
  void countEvent();
  
  void checkTopSurface();
  void checkButtomSurface();

  void   addSiteNeighbor(Site *site,Site *neighbor);
  double getRandomNumber();
  SiteType* findSiteType(char *name);
  SiteType* findSiteTypeNoAppend(char *name);
  SiteType* addSiteType(char *name);
  PathType* findPathType(SiteType *type1, SiteType *type2);
};

#endif
