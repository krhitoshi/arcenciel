#ifndef ARC_KINETIC_MC_H
#define ARC_KINETIC_MC_H

#include <iostream>
#include <string>
#include <vector>

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

  void initialize();
  void updateSystemTime();
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

  void fatalError(string message);
  void fileOpenError(string fileName);
  void clearVectors();
 public:
  KineticMC();
  bool mainProcedure();
  void printProgramName();
  void stdOutput(const char *line);
  void stdOutput(string line);
  void silentFlagOn();
  void silentFlagOff();
  void loadInputFile();
  void printInputData();
  void loadSite();
  void printCell();
  void printSiteInformation();
  void loadPath();
  void createPathToExternalPhase();
  void loadRate();
  void loadSiteType();
  void putParticles();
  void mainLoop();
  
  void checkTopSurface();
  void checkButtomSurface();

  void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time);
  void printOccurrence(int step, ostream &stream);

  void loadNumSite( const char *line);
  void loadCellParameters( const char *line);
  void loadCoordination( const char *line);
  void loadNumPath( const char *line);
  void loadPair( const char *line);
  void addSiteNeighbor(Site *site,Site *neighbor);
  double getRandomNumber();
  SiteType* findSiteType(char *name);
  SiteType* findSiteTypeNoAppend(char *name);
  SiteType* addSiteType(char *name);
  PathType* findPathType(SiteType *type1, SiteType *type2);

  void countEvent(void);
   
};

#endif
