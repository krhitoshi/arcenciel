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
  int    fileOutputInterval;         /* �ե�������ϥ��󥿡��Х� */
  int    displayOutputInterval;      /* ɸ����ϥ��󥿡��Х� */
  int    numStep;                    /* ���ƥå׿� */
  double temperature;                /* ���� */
  double systemTime, lapSystemTime;  /* ���� */
  long double sumRate;

  bool timePoisson;
  bool silentFlag;   /* ɸ����Ϥ�̵ͭ */

  int seedType;      /* ����μ� (0�ʤ�л��֤��Ȥ���)*/
  cellPara cell;     /* ����ѥ�᡼�� */


  vector<Site>     siteVector;      /* �����Ⱦ��� */
  vector<SiteType> siteTypeVector;  /* �����ȼ���� */

  vector<PathType> pathTypeVector;  /* �ѥ������ */

  vector<Particle> particleVector;  /* γ�Ҿ���   */

  vector<Event>    eventVector;     /* ���٥�Ⱦ��� */

  vector<unsigned long>    adsorptionSiteVector;
  vector<unsigned long>    desorptionSiteVector;
  vector<SitePair> dissosiativeAdsorptionSiteVector;
  vector<SitePair> recombinativeDesorptionSiteVector;

  /* ���ϥե�����̾ */
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
