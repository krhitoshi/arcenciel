#ifndef ARC_KINETIC_MC_H
#define ARC_KINETIC_MC_H

#include <iostream>
#include <string>
#include <vector>

#include "particle.h"
#include "site.h"
#include "path.h"
#include "event.h"

class KineticMC{
 private:
  int    fileOutputInterval;
  int    displayOutputInterval;
  int    numStep;
  double temperature, systemTime; /* 温度、 時間 */

  void initialize();

  bool timePoisson;

  int seedType;
  cellPara cell;

  vector<Site>     siteVector;

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
  void loadInputFile();
  void printInputData();
  void loadSite();
  void printCell();
  void printSiteInformation();
  void loadPath();
  void loadRate();
  void loadSiteType();
  void putParticles();
  void mainLoop();
  
  void checkTopSurface();
  void checkButtomSurface();

  void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time);
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
