#ifndef ARC_KINETIC_MC_H
#define ARC_KINETIC_MC_H

class KineticMC{
 private:
  int    fileOutputInterval;
  int    displayOutputInterval;
  int    numStep;
  double temperature, time; /* 温度、 時間 */

  void initialize();

  cellPara cell;
  unsigned long numSite;
  struct siteInformation *site;

  int siteTypeNum, siteTypeNumMax;
  struct siteTypeInformation *siteType;

  unsigned long numPath;

  int pathTypeNum, pathTypeNumMax;
  struct pathTypeInformation *pathType;

  int numParticle;
  struct particleInformation particle[NUM_PERTICLE];

  unsigned long numEvent, numMaxEvent;
  struct eventInformation *event;
  long double sumRate;

 public:
  KineticMC();
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
