#ifndef ARC_FUNCTIONS_H
#define ARC_FUNCTIONS_H
/*---- プロトタイプ宣言 ----*/
void fileOpenError(char *fileName);
void fatalError(char *string);

void initialize(void);
void loadInput(void);
void loadSite(void);
void loadRate(void);
void loadPath(void);

void printInputData(void);
void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time);
void mainLoop(void);
void loadNumSite( const char *line);
void loadCellParameters( const char *line);
void loadCoordination( const char *line);
void loadNumPath( const char *line);
void loadPair( const char *line);
void addSiteNeighbor(struct siteInformation *site,
	     struct siteInformation *neighbor);

struct siteTypeInformation* addSiteType(char *name);
struct siteTypeInformation* findSiteType(char *name);
struct pathTypeInformation* addPathType(struct siteTypeInformation *type1,
  struct siteTypeInformation *type2);
struct pathTypeInformation* findPathType(struct siteTypeInformation *type1,
  struct siteTypeInformation *type2);
void countEvent(void);

#endif
