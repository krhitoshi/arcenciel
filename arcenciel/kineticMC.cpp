#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <iomanip>
using namespace std;

#include "common.h"
#include "kineticMC.h"

KineticMC::KineticMC(){
  initialize();
}

void KineticMC::mainProcedure(){
  loadInputFile();
  loadSite();
  printSiteInformation();

  loadRate();
  loadPath();
  printInputData();
  putParticles();
  mainLoop();
}

/*************************************************/
/*         エラー出力、終了                      */
/*************************************************/
void KineticMC::fatalError(string message){
  cout << message << endl; 
  exit(1);
}

/*************************************************/
/*         ファイルオープンエラー出力、終了      */
/*************************************************/
void KineticMC::fileOpenError(string fileName){
  string errorString;
  errorString = "Cannot open " + fileName +"!!";
  fatalError(errorString);
}

void KineticMC::printProgramName(){
  cout << PROGRAM_NAME << endl;
  cout << "Copyright (C) 2002-2003 Hitoshi Kurokawa" << endl;
}

void KineticMC::initialize(){
  seedType              = 0;
  timePoisson           = true;
  numParticle           = 1;
  fileOutputInterval    = 50;
  displayOutputInterval = 10000;
  numStep               = 200000;

  temperature           = 300.0;

  systemTime = 0.0;

  pathTypeNum = 0;
  pathTypeNumMax = 1;
  pathType = (struct pathTypeInformation *)
    calloc(pathTypeNumMax,sizeof(struct pathTypeInformation));
  if(pathType==NULL) 
    fatalError("Cannot allocate memory for the pair types!");

  numMaxEvent = 100000;
  event = (struct eventInformation *)
    calloc(numMaxEvent,sizeof(struct eventInformation));
  if(event==NULL) 
    fatalError("Cannot allocate memory for the events!");

}

void KineticMC::loadInputFile(){
  FILE *fp;
  char fileName[]="input.kmc";
  char line[LINE], key[LINE];
  int  value;

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) 
    fileOpenError(fileName);

  while( fgets( line, LINE, fp ) != NULL ){
    sscanf(line,"%s %d",key,&value);
    /*    printf ("Key: %s Value: %d\n",key,value);*/

    if(!strcmp(key,"fileInterval"))        
      fileOutputInterval = value;
    else if(!strcmp(key,"displayInterval")) 
      displayOutputInterval = value;
    else if(!strcmp(key,"bulkParticle")) 
      numParticle = value;
    else if(!strcmp(key,"step"))        numStep            = value;
    else if(!strcmp(key,"temperature")) temperature        = value;
    else if(!strcmp(key,"poisson")){
      if(value==0)
	timePoisson = false;
      else
	timePoisson = true;
    }else if(!strcmp(key,"seed")){
      seedType = value;
    }else
      cout << "Unknown Keyword!! :" << key << endl;
  }

  if(seedType==0){
    srand(time(NULL));
  }else{
    srand(seedType);
  }

  fclose(fp);
}

/*-----------------------------------------------*/
/*         入力情報の出力                        */
/*-----------------------------------------------*/
void KineticMC::printInputData(){
  cout << "Step                     : " << numStep << endl;
  cout << "Temperature              : " << temperature << endl;
  cout << "Number of Bulk Particles : " << numParticle << endl;
  cout << "File    Output Interval  : " << fileOutputInterval << endl;
  cout << "Display Output Interval  : " << displayOutputInterval << endl;
}


/*-----------------------------------------------*/
/*         rate情報の読み込み                    */
/*-----------------------------------------------*/
void KineticMC::loadRate(){
  FILE *fp;
  char fileName[]="rate.kmc";
  char line[LINE];
  char name1[LINE],name2[LINE];
  int count,num;
  double frequency,activEnergy;
  SiteType *siteType1, *siteType2;

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) fileOpenError(fileName);
  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    num = sscanf(line, "%s %s %lf %lf",name1,name2,&frequency,&activEnergy);
    if ( num != 4 ) printf("Wrong Format in %s!!\n",fileName);
    siteType1 = findSiteType(name1);
    siteType2 = findSiteType(name2);
  struct pathTypeInformation *pathType
    = addPathType(siteType1,siteType2);
    pathType->frequency = frequency;
    pathType->activEnergy = activEnergy;
    count++;
  }
  fclose(fp);
  {
    int i;
    for(i=0; i<pathTypeNum; i++){
      pathType[i].rate = pathType[i].frequency * 
	exp(-pathType[i].activEnergy
	    *ELECTRONVOLT_TO_JOULE_PER_MOL/(GAS_CONST*temperature));
    }
  }
}

/*-----------------------------------------------*/
/*         path情報の読み込み                    */
/*-----------------------------------------------*/
void KineticMC::loadPath(){
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
    unsigned int i;
    int j;
    struct siteInformation *sitePointer, *neighbor;
    for(i=0;i<numSite;i++){
      sitePointer = &site[i];
      for(j=0;j<sitePointer->numNeighbor;j++){
	neighbor = sitePointer->neighbor[j];
	sitePointer->pathTypeToNeighbor[j] = 
	  findPathType(sitePointer->type,neighbor->type);
      }
    }
  }
  for(i=0;i<pathTypeNum;i++){
    printf("Path Type %3d: %s %s %9.5g %9.5g %9.5g\n",i,
	   pathType[i].type[0]->getName().c_str(),
	   pathType[i].type[1]->getName().c_str(),
	   pathType[i].activEnergy,
	   pathType[i].frequency,
	   pathType[i].rate	   
	   );
  }
}

void KineticMC::loadSite(){
  FILE *fp;
  char fileName[]="site.kmc";
  char line[LINE];
  int count;

  loadSiteType();

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) 
    fileOpenError(fileName);

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

  fclose(fp);

}

void KineticMC::printSiteInformation(){
  cout << "Number of Site      : " << numSite << endl;
  cout.setf(ios::fixed);
  cout.precision(4);
  
  cout << "Cell Parameters[Ang]: "
       << "      a=" << setw(10) << cell.a 
       << ",     b=" << setw(10) << cell.b
       << ",     c=" << setw(10) << cell.c << endl;
  cout << "Cell Parameters[Deg]: "
       << "  alpha=" << setw(10) << cell.alpha
       << ",  beta=" << setw(10) << cell.beta
       << ", gamma=" << setw(10) << cell.gamma << endl;

  vector<SiteType>::size_type i;
  for(i=0;i<siteTypeVector.size();i++){
    cout << "Site Type " << setw(3) << siteTypeVector[i].getNum()
	 << ": "<< siteTypeVector[i].getName() << endl;
  }
}

/*-----------------------------------------------*/
/*         site情報の読み込み                    */
/*-----------------------------------------------*/
void KineticMC::loadSiteType(){
  FILE *fp;
  char fileName[]="site.kmc";
  char line[LINE];
  int count;
  char name[LINE];
  int num;
  SiteType *siteType;
  
  if( ( fp = fopen( fileName, "r" ) ) == NULL ) fileOpenError(fileName);

  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    if( count == 1 || count == 2){
      count++;
      continue;
    }else{
      num = sscanf(line, "%s",name);
      if ( num != 1 ) printf("Wrong coordination!!\n");
      
      if(SiteType::getNumSiteType()==0){
	siteType = addSiteType(name);
      }else{
	siteType = findSiteType(name);
      }
      
    }
    count++;
  }
  fclose(fp);
}

void KineticMC::putParticles(){
  int i;
  unsigned long random;
   for(i=0;i<numParticle;i++){
    random = (unsigned long)
      (getRandomNumber()*numSite);
    if(site[random].state==UNOCCUPY){
      site[random].state=OCCUPY;
      particle[i].site = &site[random];
    }else{
      i--;
    }
  }

}

/*-----------------------------------------------*/
/*         メインループ                          */
/*-----------------------------------------------*/

void KineticMC::mainLoop(){
  FILE *fp_out, *fp_time;
  long double eventRandom;
  int step;
  unsigned int i;  
  if( ( fp_out = fopen( "out.kmc", "w" )  ) == NULL ) 
    fileOpenError("out.kmc");
  if( ( fp_time = fopen( "time.kmc", "w" )) == NULL ) 
    fileOpenError("time.kmc");
  cout << "############## Start ##############\n";


  printIntervalOutput(0,fp_out, fp_time);

  /*---- ループスタート ----*/
  cout << "  STEP      TIME\n";
  for(step=1;step<numStep+1;step++){
    countEvent();
    eventRandom = getRandomNumber()*sumRate;
    /* printf ("Number of Event: %lu %Lf %Lf\n",numEvent,sumRate,eventRandom);*/
    //    cout << "Randnum " << eventRandom << endl;
    for(i=0;i<numEvent;i++){
      eventRandom -= event[i].rate;
      if(eventRandom < 0.0) break;
    }

    if(timePoisson){
      systemTime += -log(getRandomNumber())/sumRate;
    }else{
      systemTime += 1.0/sumRate;
    }

    /*    printf ("%u th event!! Time: %e\n",i,systemTime);*/
    event[i].currentSite->state = UNOCCUPY;
    event[i].nextSite->state = OCCUPY;
    event[i].particle->site = event[i].nextSite;

    if(step!=0&&step%displayOutputInterval==0)
      printf ("%10d %10.5e\n",step,systemTime);

    if(step!=0&&step%fileOutputInterval==0)
      printIntervalOutput(step,fp_out, fp_time);
  }
  /*---- ループエンド ----*/
  fclose(fp_out);
  fclose(fp_time);
  cout << "##############  End  ##############\n";
}

/*-----------------------------------------------*/
/*         イベントのカウント                     */
/*-----------------------------------------------*/
void  KineticMC::countEvent(){
  int num,i;
  int numNeighbor;
  struct siteInformation *sitePointer;
  struct eventInformation *eventPointer;
  numEvent=0;
  sumRate=0.0;
  for(num=0;num<numParticle;num++){
    sitePointer = particle[num].site;
    numNeighbor = sitePointer->numNeighbor;
    for (i=0; i< sitePointer->numNeighbor; i++){
      if(sitePointer->neighbor[i]->state==OCCUPY) numNeighbor--;
    }
    
    for (i=0; i< sitePointer->numNeighbor; i++){
      if(sitePointer->neighbor[i]->state==OCCUPY) continue;
      numEvent++;
      eventPointer = &event[numEvent-1];

      eventPointer->rate =  sitePointer->pathTypeToNeighbor[i]->rate/numNeighbor;

      sumRate += eventPointer->rate;
      eventPointer->particle = &particle[num];
      eventPointer->currentSite = sitePointer;
      eventPointer->nextSite = sitePointer->neighbor[i];
    }
  }
}


/*-----------------------------------------------*/
/*         入力情報の出力                        */
/*-----------------------------------------------*/
void  KineticMC::printIntervalOutput(int step, FILE *fp_out, FILE *fp_time){
  int i;
  fprintf (fp_time,"%10d %10.5e\n",step,systemTime);
  fflush(fp_time);
  
  for(i=1;i<numParticle+1;i++){
    if(i==1) fprintf (fp_out,"%5d ",(int)(step/fileOutputInterval));
    else if((i%10)==1) 
      fprintf (fp_out,"\n%5d ",(int)(step/fileOutputInterval));
    
    fprintf (fp_out,"%5lu ",particle[i-1].site->num);
  }
  fprintf (fp_out,"\n");
  fflush(fp_out);
}


/*-----------------------------------------------*/
/*         site数の読み込み                      */
/*-----------------------------------------------*/
void  KineticMC::loadNumSite( const char *line){
  int num;
  num = sscanf( line, "%lu",&numSite);
  if ( num != 1 ) 
    fatalError("Not Find the number of sites!");
  site = (struct siteInformation *)
    calloc(numSite,sizeof(struct siteInformation));
  if(site==NULL) 
    fatalError("Cannot allocate memory for the sites!");
}

/*-----------------------------------------------*/
/*         セルパラメータの読み込み            */
/*-----------------------------------------------*/
void  KineticMC::loadCellParameters( const char *line){
  int num;
  num = sscanf( line, "%f %f %f %f %f %f"
		,&cell.a,&cell.b,&cell.c
		,&cell.alpha,&cell.beta,&cell.gamma);
  if ( num != 6 ) 
    fatalError("The number of cell parameters is wrong!\nIt should be 6!");

}

/*-----------------------------------------------*/
/*         サイトの座標の読み込み                */
/*-----------------------------------------------*/
void  KineticMC::loadCoordination( const char *line){
  static unsigned long siteCount;
  struct position3D pos;
  char name[LINE];
  int num;

  num = sscanf(line, "%s %f %f %f",name,&pos.x,&pos.y,&pos.z);
  if ( num != 4 ) printf("Wrong coordination!!\n");

  SiteType *foundSiteType = findSiteType(name);
  struct siteInformation *newSite = &site[siteCount];

  newSite->num = siteCount;
  newSite->pos = pos;
  newSite->type = foundSiteType;
  newSite->state = UNOCCUPY;
  newSite->numNeighbor = 0;

  siteCount++;
}


/*-----------------------------------------------*/
/*         path数の読み込み                      */
/*-----------------------------------------------*/
void  KineticMC::loadNumPath( const char *line){
  int num;
  num = sscanf( line, "%lu",&numPath);
  if ( num != 1 ) 
    fatalError("Not Find the number of paths!");
}

/*-----------------------------------------------*/
/*         隣接サイトの追加                      */
/*-----------------------------------------------*/
void  KineticMC::addSiteNeighbor(struct siteInformation *site,
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
void  KineticMC::loadPair( const char *line){
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
  numPair = (int)((double)num/2.0);
  for(i=0;i<numPair;i++){
    site1 = &site[pair[i][0]];
    site2 = &site[pair[i][1]];

    if(pathTypeNum==0){
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
SiteType*  KineticMC::addSiteType(char *name){
  siteTypeVector.push_back(SiteType(name));

  /*  printf ("%d %s \n",siteType[siteTypeNum-1].num,
      siteType[siteTypeNum-1].name);*/
  return &siteTypeVector[SiteType::getNumSiteType()-1];
}

/*-----------------------------------------------*/
/*         pair typeの追加                       */
/*-----------------------------------------------*/
struct pathTypeInformation*  KineticMC::addPathType(SiteType *type1
				 , SiteType *type2){

  struct pathTypeInformation *newPathType;
  pathTypeNum++;
  if(pathTypeNum>pathTypeNumMax){
    int initNumMax, i;
    initNumMax = pathTypeNumMax;
    pathTypeNumMax += 10;
    pathType = (struct pathTypeInformation *)
      realloc(pathType,pathTypeNumMax*sizeof(struct pathTypeInformation));
    if(pathType==NULL) 
      fatalError("Cannot allocate memory for the pair types!");
    for(i=initNumMax;i<pathTypeNumMax;i++){
      pathType[i].num = 0;
      pathType[i].type[0] = NULL;
      pathType[i].type[1] = NULL;
    }
  }
  newPathType = &pathType[pathTypeNum-1];
  newPathType->type[0] = type1;
  newPathType->type[1] = type2;

  return newPathType;  
}

/*-----------------------------------------------*/
/*         site typeの検索                       */
/*-----------------------------------------------*/
SiteType*  KineticMC::findSiteType(char *name){
  int i;
  for(i=0;i<SiteType::getNumSiteType();i++){
    if(string(name) == siteTypeVector[i].getName()) 
      return &siteTypeVector[i];
  }
  cout << "Adding a new site type" << endl;
  return addSiteType(name);  
}

/*-----------------------------------------------*/
/*         pair typeの検索                       */
/*-----------------------------------------------*/
struct pathTypeInformation*  KineticMC::findPathType
(SiteType *type1
 ,SiteType *type2){
  int i;
  for(i=0;i<pathTypeNum;i++){
    if((pathType[i].type[0]->getNum()==type1->getNum()
	&& pathType[i].type[1]->getNum()==type2->getNum()))
      return &pathType[i];
  }
  printf ("Cannot find!! path type %s %s\n",
	  type1->getName().c_str(),type2->getName().c_str());
  return addPathType(type1,type2);  
}


/*-----------------------------------------------*/
/*         乱数の発生                       */
/*-----------------------------------------------*/
double KineticMC::getRandomNumber(){
  return (double)rand()/(double)RAND_MAX;
}
