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
  double temperature, systemTime; /* ���١� ���� */

  void initialize();

  bool timePoisson;

  int seedType;
  cellPara cell;
  unsigned long numSite;
  struct siteInformation *site;

  vector<siteTypeInformation> siteTypeVector;

  int siteTypeNum, siteTypeNumMax;
  struct siteTypeInformation *siteType;

  unsigned long numPath;

  int pathTypeNum, pathTypeNumMax;
  struct pathTypeInformation *pathType;

  int numParticle;
  struct particleInformation particle[MAX_NUM_PERTICLE];

  unsigned long numEvent, numMaxEvent;
  struct eventInformation *event;
  long double sumRate;

  void fatalError(string message);
  void fileOpenError(string fileName);

 public:
  KineticMC();
  void printProgramName();
  void loadInputFile();
  void printInputData();
  void loadSite();
  void loadPath();
  void loadRate();
  void loadSiteType();
  void putParticles();
  void mainLoop();

  void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time);
  void loadNumSite( const char *line);
  void loadCellParameters( const char *line);
  void loadCoordination( const char *line);
  void loadNumPath( const char *line);
  void loadPair( const char *line);
  void addSiteNeighbor(struct siteInformation *site,
	     struct siteInformation *neighbor);
  double getRandomNumber();
  struct siteTypeInformation* findSiteType(char *name);
  struct siteTypeInformation* addSiteType(char *name);

  struct pathTypeInformation*
    findPathType(struct siteTypeInformation *type1
		 , struct siteTypeInformation *type2);

  struct pathTypeInformation* 
    addPathType(struct siteTypeInformation *type1
		, struct siteTypeInformation *type2);

  void countEvent(void);
   
};

#endif
