/*****************************************************************************/
/*                Kinetic Monte Carlo Program                                */
/*                                                                           */
/*             Copyright (C) 2002 Hitoshi Kurokawa                           */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define LINE       1024
#define NAME_LIMIT 10

#define AVOGADRO_CONST  6.0221367e23 /* [/mol] */
#define BOLTZMANN_CONST 1.380658e-23 /* [J/K] */
#define ELECTRONVOLT_TO_JOULE_PER_MOL 9.648531e4 /* [eV] -> [J/mol] */
#define GAS_CONST AVOGADRO_CONST*BOLTZMANN_CONST /* [J/(mol*K)] */
void fileOpenError(char *fileName);
void fatalError(char *string);

/*---- 座標 ----*/
typedef struct position3D{
  float x,y,z;
}pos3D;

/*---- セルパラメーター ----*/
typedef struct cellParameters{
  float a,b,c,alpha,beta,gamma;
}cellPara;

/*---- サイトの状態 ----*/
enum siteState {  UNOCCUPY, OCCUPY};

/*---- サイト情報 ----*/
struct siteInformation{
  unsigned long num;                /* 番号 */
  struct siteTypeInformation *type; /* 種類 */
  pos3D pos;                        /* 座標 */
  enum siteState state;             /* 状態 */
  int numNeighbor;                  /* 隣接するサイトの数 */
  struct siteInformation **neighbor; /* 隣接するサイトのポインタの配列 */
  struct pathTypeInformation **pathTypeToNeighbor;
};

/*---- サイトの種類 ----*/
struct siteTypeInformation{
  int num;
  char name[NAME_LIMIT];
};

/*---- パスの種類 ----*/
struct pathTypeInformation{
  int num;
  struct siteTypeInformation *type[2];
  double activEnergy, frequency, rate;
};

/*---- 粒子情報 ----*/
struct particleInformation{
  struct siteInformation *site;
};

/*---- イベント情報 ----*/
enum eventType{ DIFFUSION, ABSORPTION, DESORPTION };
struct eventInformation{
  enum eventType type;
  double rate;
  struct siteInformation *currentSite;
  struct siteInformation *nextSite;
  struct particleInformation *particle;
};

#define NUM_PERTICLE 50

/*---- 基本情報 ----*/
struct basicInformation{
  int outputInterval;
  int numStep;

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

  double temperature, time; /* 温度、 時間 */
}kmc;

/*---- プロトタイプ宣言 ----*/

void initialize();
void loadInput();
void loadSite();
void loadRate();
void loadPath();

void printInputData();
void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time);
void mainLoop();
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

float rate;

/*************************************************/
/*         メイン                                */
/*************************************************/
int main(){
  int i;
  unsigned long random;

  initialize();
  loadInput();
  loadSite();
  loadRate();
  loadPath();

  /*  rate = 2.22e13*exp(-21.80*1000.0/(GAS_CONST*kmc.temperature));*/
  rate = 2.22e13; 
  kmc.numParticle = NUM_PERTICLE;

  printInputData();

  for(i=0;i<kmc.numParticle;i++){
    random = (double)rand()/(double)RAND_MAX*kmc.numSite;
    if(kmc.site[random].state==UNOCCUPY){
      kmc.site[random].state=OCCUPY;
      kmc.particle[i].site = &kmc.site[random];
    }
  }
 
  mainLoop();

  return 0;
}

/*-----------------------------------------------*/
/*         メインループ                          */
/*-----------------------------------------------*/

void mainLoop(){
  FILE *fp_out, *fp_time;
  long double eventRandom;
  int step;
  int i;  
  if( ( fp_out = fopen( "out.kmc", "w" )  ) == NULL ) fileOpenError("out.kmc");
  if( ( fp_time = fopen( "time.kmc", "w" )) == NULL ) fileOpenError("time.kmc");

  printIntervalOutput(0,fp_out, fp_time);

  /*---- ループスタート ----*/
  for(step=1;step<kmc.numStep+1;step++){
    
    countEvent();
    eventRandom = (double)rand()/(double)RAND_MAX*kmc.sumRate;
    /*      printf ("Number of Event: %lu %Lf %Lf\n",kmc.numEvent,kmc.sumRate,eventRandom);*/
    for(i=0;i<kmc.numEvent;i++){
      eventRandom -= kmc.event[i].rate;
      if(eventRandom < 0.0) break;
    }

    /*      kmc.time += -log((double)rand()/(double)RAND_MAX)*1.0/kmc.sumRate; */
    /*      kmc.time += -log((double)rand()/(double)RAND_MAX)*1.0/kmc.sumRate*12.0; */
    kmc.time += 1.0/kmc.sumRate;

    /*    printf ("%u th event!! Time: %e\n",i,kmc.time);*/
    kmc.event[i].currentSite->state = UNOCCUPY;
    kmc.event[i].nextSite->state = OCCUPY;
    kmc.event[i].particle->site = kmc.event[i].nextSite;

    if(step!=0&&step%kmc.outputInterval==0)
      printIntervalOutput(step,fp_out, fp_time);
  }
  /*---- ループエンド ----*/
  fclose(fp_out);
  fclose(fp_time);
}

/*-----------------------------------------------*/
/*         イベントのカウント                     */
/*-----------------------------------------------*/
void countEvent(){
  unsigned long num,i;
  int numNeighbor;
  struct siteInformation *site;
  struct eventInformation *event;
  kmc.numEvent=0;
  kmc.sumRate=0.0;
  for(num=0;num<kmc.numParticle;num++){
    site = kmc.particle[num].site;
    numNeighbor = site->numNeighbor;
    for (i=0; i< site->numNeighbor; i++){
      if(site->neighbor[i]->state==OCCUPY) numNeighbor--;
    }
    
    for (i=0; i< site->numNeighbor; i++){
      if(site->neighbor[i]->state==OCCUPY) continue;
      kmc.numEvent++;
      event = &kmc.event[kmc.numEvent-1];

      event->rate =  site->pathTypeToNeighbor[i]->rate/numNeighbor;

      kmc.sumRate += event->rate;
      event->particle = &kmc.particle[num];
      event->currentSite = site;
      event->nextSite = site->neighbor[i];
    }
  }
}

/*-----------------------------------------------*/
/*         初期化                                */
/*-----------------------------------------------*/
void initialize(){

  kmc.time = 0.0;

  kmc.siteTypeNum = 0;
  kmc.siteTypeNumMax = 1;
  kmc.siteType = (struct siteTypeInformation *)
    calloc(kmc.siteTypeNumMax,sizeof(struct siteTypeInformation));
  if(kmc.siteType==NULL) 
    fatalError("Cannot allocate memory for the site types!");

  kmc.pathTypeNum = 0;
  kmc.pathTypeNumMax = 1;
  kmc.pathType = (struct pathTypeInformation *)
    calloc(kmc.pathTypeNumMax,sizeof(struct pathTypeInformation));
  if(kmc.pathType==NULL) 
    fatalError("Cannot allocate memory for the pair types!");

  kmc.numMaxEvent = 100000;
  kmc.event = (struct eventInformation *)
    calloc(kmc.numMaxEvent,sizeof(struct eventInformation));
  if(kmc.event==NULL) 
    fatalError("Cannot allocate memory for the events!");

  /* インプットデータのデフォルト値 */
  kmc.outputInterval = 50;
  kmc.numStep = 10000;
  kmc.temperature = 300;
}

/*-----------------------------------------------*/
/*         入力情報の出力                        */
/*-----------------------------------------------*/
void printInputData(){
  printf("Step        :%d\n",kmc.numStep);
  printf("Interval    :%d\n",kmc.outputInterval);
  printf("Temperature :%7.4f\n",kmc.temperature);
  printf("%7d Particles\n",kmc.numParticle);
}

/*-----------------------------------------------*/
/*         入力情報の出力                        */
/*-----------------------------------------------*/
void printIntervalOutput(int step, FILE *fp_out, FILE *fp_time){
  int i;
  printf ("STEP: %10d TIME %10.5e\n",step,kmc.time);
  fprintf (fp_time,"%10d %10.5e\n",step,kmc.time);
  fflush(fp_time);
  
  for(i=1;i<kmc.numParticle+1;i++){
    if(i==1) fprintf (fp_out,"%5d ",(int)(step/kmc.outputInterval));
    else if((i%10)==1) fprintf (fp_out,"\n%5d ",(int)(step/kmc.outputInterval));
    
    fprintf (fp_out,"%5lu ",kmc.particle[i-1].site->num);
  }
  fprintf (fp_out,"\n");
  fflush(fp_out);
}
/*-----------------------------------------------*/
/*         インプットデータの読み込み            */
/*-----------------------------------------------*/
void loadInput(){
  FILE *fp;
  char fileName[]="input.kmc";
  char line[LINE], key[LINE];
  int  value;

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) fileOpenError(fileName);
  while( fgets( line, LINE, fp ) != NULL ){
    sscanf(line,"%s %d",key,&value);
    /*    printf ("Key: %s Value: %d\n",key,value);*/
    if(!strcmp(key,"interval"))  kmc.outputInterval = value;
    else if(!strcmp(key,"step"))  kmc.numStep       = value;
    else if(!strcmp(key,"temperature"))  kmc.temperature = value;
  }
  fclose(fp);
}

/*-----------------------------------------------*/
/*         site情報の読み込み                    */
/*-----------------------------------------------*/
void loadSiteType(){
  FILE *fp;
  char fileName[]="site.kmc";
  char line[LINE];
  int count;
  char name[NAME_LIMIT];
  int num;
  struct siteTypeInformation *siteType;
  
  if( ( fp = fopen( fileName, "r" ) ) == NULL ) fileOpenError(fileName);

  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    if( count == 1 || count == 2){
      count++;
      continue;
    }else{
      num = sscanf(line, "%s",name);
      if ( num != 1 ) printf("Wrong coordination!!\n");
      
      if(kmc.siteTypeNum==0){
	siteType = addSiteType(name);
      }else{
	siteType = findSiteType(name);
      }
      
    }
    count++;
  }
  fclose(fp);
}

void loadSite(){
  FILE *fp;
  char fileName[]="site.kmc";
  char line[LINE];
  int count;

  loadSiteType();

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) fileOpenError(fileName);

  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    if( count == 1 )
      loadNumSite(line);
    else if( count == 2 )
      loadCellParameters(line);
    else
      loadCoordination(line);
    count++;
  }

  {
    int j;
    printf("Number of Site : %lu\n",kmc.numSite);
    printf("Cell Parameters[Ang]:     a=%7.4f    b=%7.4f     c=%7.4f\n"
	   ,kmc.cell.a,kmc.cell.b,kmc.cell.c);
    printf("Cell Parameters[Deg]: alpha=%7.4f beta=%7.4f gamma=%7.4f\n"
	   ,kmc.cell.alpha,kmc.cell.beta,kmc.cell.gamma);
    for(j=0;j<kmc.siteTypeNum;j++){
      printf("Site Type %3d: %s\n",kmc.siteType[j].num,kmc.siteType[j].name);
    }
  }
  fclose(fp);
}

/*-----------------------------------------------*/
/*         site数の読み込み                      */
/*-----------------------------------------------*/
void loadNumSite( const char *line){
  int num;
  num = sscanf( line, "%lu",&kmc.numSite);
  if ( num != 1 ) 
    fatalError("Not Find the number of sites!");
  kmc.site = (struct siteInformation *)
    calloc(kmc.numSite,sizeof(struct siteInformation));
  if(kmc.site==NULL) 
    fatalError("Cannot allocate memory for the sites!");
}

/*-----------------------------------------------*/
/*         セルパラメータの読み込み            */
/*-----------------------------------------------*/
void loadCellParameters( const char *line){
  int num;
  num = sscanf( line, "%f %f %f %f %f %f"
		,&kmc.cell.a,&kmc.cell.b,&kmc.cell.c
		,&kmc.cell.alpha,&kmc.cell.beta,&kmc.cell.gamma);
  if ( num != 6 ) 
    fatalError("The number of cell parameters is wrong!\nIt should be 6!");

}

/*-----------------------------------------------*/
/*         サイトの座標の読み込み                */
/*-----------------------------------------------*/
void loadCoordination( const char *line){
  static unsigned long siteCount;
  struct position3D pos;
  char name[NAME_LIMIT];
  int num;
  struct siteInformation *site;
  struct siteTypeInformation *siteType;

  num = sscanf(line, "%s %f %f %f",name,&pos.x,&pos.y,&pos.z);
  if ( num != 4 ) printf("Wrong coordination!!\n");

  siteType = findSiteType(name);
  site = &kmc.site[siteCount];
  site->num = siteCount;
  site->pos = pos;
  site->type = siteType;
  site->state = UNOCCUPY;
  site->numNeighbor = 0;

  siteCount++;
}

/*-----------------------------------------------*/
/*         rate情報の読み込み                    */
/*-----------------------------------------------*/
void loadRate(){
  FILE *fp;
  char fileName[]="rate.kmc";
  char line[LINE];
  char name1[NAME_LIMIT],name2[NAME_LIMIT];
  int count,num;
  double frequency,activEnergy;
  struct siteTypeInformation *siteType1, *siteType2;
  struct pathTypeInformation *pathType;
  if( ( fp = fopen( fileName, "r" ) ) == NULL ) fileOpenError(fileName);
  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    num = sscanf(line, "%s %s %lf %lf",name1,name2,&frequency,&activEnergy);
    if ( num != 4 ) printf("Wrong Format in %s!!\n",fileName);
    siteType1 = findSiteType(name1);
    siteType2 = findSiteType(name2);
    pathType = addPathType(siteType1,siteType2);
    pathType->frequency = frequency;
    pathType->activEnergy = activEnergy;
    count++;
  }
  fclose(fp);
  {
    int i;
    for(i=0; i<kmc.pathTypeNum; i++){
      kmc.pathType[i].rate = kmc.pathType[i].frequency * 
	exp(-kmc.pathType[i].activEnergy
	    *ELECTRONVOLT_TO_JOULE_PER_MOL/(GAS_CONST*kmc.temperature));
    }
  }
}

/*-----------------------------------------------*/
/*         path情報の読み込み                    */
/*-----------------------------------------------*/
void loadPath(){
  FILE *fp;
  char fileName[]="path.kmc";
  char line[LINE];
  int count,i;

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) fileOpenError(fileName);

  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    if( count == 1 )
      loadNumPath(line);
    else
      loadPair(line);
    count++;
  }
  {
    int i,j;
    struct siteInformation *site, *neighbor;
    for(i=0;i<kmc.numSite;i++){
      site = &kmc.site[i];
      for(j=0;j<site->numNeighbor;j++){
	neighbor = site->neighbor[j];
	site->pathTypeToNeighbor[j] = 
	  findPathType(site->type,neighbor->type);
      }
    }
  }
  for(i=0;i<kmc.pathTypeNum;i++){
    printf("Path Type %3d: %s %s %9.5g %9.5g %9.5g\n",i,
	   kmc.pathType[i].type[0]->name,
	   kmc.pathType[i].type[1]->name,
	   kmc.pathType[i].activEnergy,
	   kmc.pathType[i].frequency,
	   kmc.pathType[i].rate	   
	   );
  }
}

/*-----------------------------------------------*/
/*         path数の読み込み                      */
/*-----------------------------------------------*/
void loadNumPath( const char *line){
  int num;
  num = sscanf( line, "%lu",&kmc.numPath);
  if ( num != 1 ) 
    fatalError("Not Find the number of paths!");
}

/*-----------------------------------------------*/
/*         隣接サイトの追加                      */
/*-----------------------------------------------*/
void addSiteNeighbor(struct siteInformation *site,
	     struct siteInformation *neighbor){

    site->numNeighbor++;

    site->neighbor = (struct siteInformation **)
      realloc(site->neighbor
	      ,site->numNeighbor*sizeof(struct siteInformation*));

    site->pathTypeToNeighbor = (struct pathTypeInformation **)
      realloc(site->pathTypeToNeighbor
	   ,site->numNeighbor*sizeof(struct pathTypeInformation*));

    if(site->neighbor==NULL) 
      fatalError("Cannot allocate memory for the sites [neighbor]!");
    if(site->pathTypeToNeighbor==NULL) 
      fatalError("Cannot allocate memory for the path types to neighbors!");

    site->neighbor[site->numNeighbor-1] = neighbor;
}

/*-----------------------------------------------*/
/*         path(siteのペア)の読み込み            */
/*-----------------------------------------------*/
void loadPair( const char *line){
  int i,num,numPair;
  unsigned long pair[10][2];
  static unsigned long pathCount;
  struct siteInformation *site1, *site2;
  struct pathTypeInformation *pathType;
  num = sscanf(line, 
      "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"
   ,&pair[0][0],&pair[0][1],&pair[1][0],&pair[1][1],&pair[2][0],&pair[2][1]
   ,&pair[3][0],&pair[3][1],&pair[4][0],&pair[4][1],&pair[5][0],&pair[5][1]
   ,&pair[6][0],&pair[6][1],&pair[7][0],&pair[7][1],&pair[8][0],&pair[8][1]
   ,&pair[9][0],&pair[9][1]);
      
  if(num%2!=0) printf("Wrong path!!\n");
  numPair = (double)num/2.0;
  for(i=0;i<numPair;i++){
    site1 = &kmc.site[pair[i][0]];
    site2 = &kmc.site[pair[i][1]];

    if(kmc.pathTypeNum==0){
      pathType = addPathType(site1->type,site2->type);
    }else{
      pathType = findPathType(site1->type,site2->type);
    }

    addSiteNeighbor(site1,site2);
    addSiteNeighbor(site2,site1);

    pathCount++;
  }
}


/*-----------------------------------------------*/
/*         site typeの追加                       */
/*-----------------------------------------------*/
struct siteTypeInformation* addSiteType(char *name){
  kmc.siteTypeNum++;
  if(kmc.siteTypeNum>kmc.siteTypeNumMax){
    int initNumMax, i;
    initNumMax = kmc.siteTypeNumMax;
    kmc.siteTypeNumMax += 10;
    kmc.siteType = (struct siteTypeInformation *)
      realloc(kmc.siteType,kmc.siteTypeNumMax*sizeof(struct siteTypeInformation));
    if(kmc.siteType==NULL) 
      fatalError("Cannot allocate memory for the site types!");

    for(i=initNumMax;i<kmc.siteTypeNumMax;i++){
      kmc.siteType[i].num = 0;
      kmc.siteType[i].name[0] = '\0';
    }
  }
  strncpy(kmc.siteType[kmc.siteTypeNum-1].name,name,NAME_LIMIT);
  kmc.siteType[kmc.siteTypeNum-1].num = kmc.siteTypeNum-1;

  /*  printf ("%d %s \n",kmc.siteType[kmc.siteTypeNum-1].num,
      kmc.siteType[kmc.siteTypeNum-1].name);*/
  return &kmc.siteType[kmc.siteTypeNum-1];
}

/*-----------------------------------------------*/
/*         pair typeの追加                       */
/*-----------------------------------------------*/
struct pathTypeInformation* addPathType(struct siteTypeInformation *type1
				 , struct siteTypeInformation *type2){

  struct pathTypeInformation *pathType;
  kmc.pathTypeNum++;
  if(kmc.pathTypeNum>kmc.pathTypeNumMax){
    int initNumMax, i;
    initNumMax = kmc.pathTypeNumMax;
    kmc.pathTypeNumMax += 10;
    kmc.pathType = (struct pathTypeInformation *)
      realloc(kmc.pathType,kmc.pathTypeNumMax*sizeof(struct pathTypeInformation));
    if(kmc.pathType==NULL) 
      fatalError("Cannot allocate memory for the pair types!");
    for(i=initNumMax;i<kmc.pathTypeNumMax;i++){
      kmc.pathType[i].num = 0;
      kmc.pathType[i].type[0] = NULL;
      kmc.pathType[i].type[1] = NULL;
    }
  }
  pathType = &kmc.pathType[kmc.pathTypeNum-1];
  pathType->type[0] = type1;
  pathType->type[1] = type2;

  return pathType;  
}

/*-----------------------------------------------*/
/*         site typeの検索                       */
/*-----------------------------------------------*/
struct siteTypeInformation* findSiteType(char *name){
  int i;
  for(i=0;i<kmc.siteTypeNum;i++){
    if(strcmp(name,kmc.siteType[i].name)==0) return &kmc.siteType[i];
  }
  printf ("Cannot find!! site type\n");
  return addSiteType(name);  
}

/*-----------------------------------------------*/
/*         pair typeの検索                       */
/*-----------------------------------------------*/
struct pathTypeInformation* findPathType(struct siteTypeInformation *type1
				 , struct siteTypeInformation *type2){
  int i;
  for(i=0;i<kmc.pathTypeNum;i++){
    if((kmc.pathType[i].type[0]->num==type1->num
	&& kmc.pathType[i].type[1]->num==type2->num))
      return &kmc.pathType[i];
  }
  printf ("Cannot find!! path type %s %s\n",type1->name,type2->name);
  return addPathType(type1,type2);  
}

/*************************************************/
/*         エラー出力、終了                      */
/*************************************************/
void fatalError(char *string)
{
  fprintf( stderr, "%s\n",string);
  exit(1);
}

/*************************************************/
/*         ファイルオープンエラー出力、終了      */
/*************************************************/
void fileOpenError(char *fileName)
{
  char errorString[LINE];
  
  sprintf(errorString, "Cannot open %s!!",fileName);
  fatalError(errorString);
}
