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
  int  num;
  double frequency,activEnergy;
  SiteType *siteType1, *siteType2;

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) 
    fileOpenError(fileName);

  while( fgets( line, LINE, fp ) != NULL ){
    num = sscanf(line, "%s %s %lf %lf",
		 name1,name2,&frequency,&activEnergy);
    if ( num != 4 ) printf("Wrong Format in %s!!\n",fileName);
    
    siteType1 = findSiteType(name1);
    siteType2 = findSiteType(name2);
    
    pathTypeVector.push_back(PathType(siteType1,siteType2,
			    frequency,activEnergy,temperature));
  }
  fclose(fp);
}

/*-----------------------------------------------*/
/*         path情報の読み込み                    */
/*-----------------------------------------------*/
void KineticMC::loadPath(){
  FILE *fp;
  char fileName[]="path.kmc";
  char line[LINE];
  int count;

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
    vector<Site>::size_type i;
    int j;
    Site *sitePointer, *neighbor;
    for(i=0;i<siteVector.size();i++){
      sitePointer = &siteVector[i];
      for(j=0;j<sitePointer->getNumNeighbor();j++){
	neighbor = sitePointer->getNeighbor(j);
	sitePointer->setPathTypeToNeighbor(j,
				   findPathType(sitePointer->getType(), neighbor->getType()));
      }
    }
  }
  vector<PathType>::size_type k;
  for(k=0;k<pathTypeVector.size();k++){
    pathTypeVector[k].print();
  }
}

void KineticMC::loadSite(){
  FILE *fp;
  char fileName[]="site.kmc";
  char line[LINE];
  int count;

  cout << "loading " << fileName << "..." << endl;
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
  cout << "Number of Site      : " << siteVector.size() << endl;
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
      (getRandomNumber()*siteVector.size());
    if(siteVector[random].getState()==Site::UNOCCUPY){
      siteVector[random].setState(Site::OCCUPY);
      particleVector.push_back(&siteVector[random]);
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
  vector<Event>::size_type i;  
  if( ( fp_out = fopen( "out.kmc", "w" )  ) == NULL ) 
    fileOpenError("out.kmc");
  if( ( fp_time = fopen( "time.kmc", "w" )) == NULL ) 
    fileOpenError("time.kmc");
  cout << "############## Start ##############\n";


  printIntervalOutput(0,fp_out, fp_time);

  /*---- ループスタート ----*/
  cout << "  STEP      TIME\n";
  for(step=1;step<numStep+1;step++){
    if(eventVector.size()!=0) eventVector.clear();
    countEvent();
    eventRandom = getRandomNumber()*sumRate;
    /* printf ("Number of Event: %lu %Lf %Lf\n",numEvent,sumRate,eventRandom);*/
    //    cout << "Randnum " << eventRandom << endl;
    for(i=0;i<eventVector.size();i++){
      eventRandom -= eventVector[i].getRate();
      if(eventRandom < 0.0) break;
    }

    if(timePoisson){
      systemTime += -log(getRandomNumber())/sumRate;
    }else{
      systemTime += 1.0/sumRate;
    }

    /*    printf ("%u th event!! Time: %e\n",i,systemTime);*/
    eventVector[i].getCurrentSite()->setState(Site::UNOCCUPY);
    eventVector[i].getNextSite()->setState(Site::OCCUPY);
    eventVector[i].getParticle()
      ->setSite(eventVector[i].getNextSite());

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
  vector<Particle>::size_type num;
  int i;
  int numNeighbor, saveNumNeighbor;
  Site *sitePointer;
  sumRate=0.0;
  double rate=0.0;

  for(num=0;num<particleVector.size();num++){

    sitePointer = particleVector[num].getSite();

    saveNumNeighbor = sitePointer->getNumNeighbor();
    numNeighbor     = sitePointer->getRealNumNeighbor();

    for (i=0; i< saveNumNeighbor; i++){
      if(sitePointer->getNeighbor(i)->getState()==Site::OCCUPY)
	continue;

      rate = 
	sitePointer->getPathTypeToNeighbor(i)->getRate()/numNeighbor;

      eventVector.push_back(Event(rate,&particleVector[num],
			  sitePointer,sitePointer->getNeighbor(i)));

      sumRate += rate;

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
    
    fprintf (fp_out,"%5lu ",particleVector[i-1].getSite()->getNum());
  }
  fprintf (fp_out,"\n");
  fflush(fp_out);
}


/*-----------------------------------------------*/
/*         site数の読み込み                      */
/*-----------------------------------------------*/
void  KineticMC::loadNumSite( const char *line){
  int num;
  unsigned long dummy;
  num = sscanf( line, "%lu",&dummy);
  if ( num != 1 ) 
    fatalError("Not Find the number of sites!");

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
  struct position3D pos;
  char name[LINE];
  int num;

  num = sscanf(line, "%s %f %f %f",name,&pos.x,&pos.y,&pos.z);
  if ( num != 4 ) printf("Wrong coordination!!\n");

  SiteType *foundSiteType = findSiteType(name);

  siteVector.push_back(Site(pos,foundSiteType));
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
void  KineticMC::addSiteNeighbor(Site *site, Site *neighbor){


}

/*-----------------------------------------------*/
/*         path(siteのペア)の読み込み            */
/*-----------------------------------------------*/
void  KineticMC::loadPair( const char *line){
  int i,num,numPair;
  unsigned long pair[10][2];
  Site *site1, *site2;

  num = sscanf(line, 
      "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"
   ,&pair[0][0],&pair[0][1],&pair[1][0],&pair[1][1],&pair[2][0],&pair[2][1]
   ,&pair[3][0],&pair[3][1],&pair[4][0],&pair[4][1],&pair[5][0],&pair[5][1]
   ,&pair[6][0],&pair[6][1],&pair[7][0],&pair[7][1],&pair[8][0],&pair[8][1]
   ,&pair[9][0],&pair[9][1]);
      
  if(num%2!=0) printf("Wrong path!!\n");
  numPair = (int)((double)num/2.0);
  for(i=0;i<numPair;i++){
    site1 = &siteVector[pair[i][0]];
    site2 = &siteVector[pair[i][1]];

    
    site1->addNeighbor(site2,
		    findPathType(site1->getType(),site2->getType()));
    site2->addNeighbor(site1,
		    findPathType(site2->getType(),site1->getType()));
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
PathType*  KineticMC::findPathType(SiteType *type1,SiteType *type2){
  vector<PathType>::size_type i;

  for(i=0;i<pathTypeVector.size();i++){
    if(pathTypeVector[i].getSiteType1()->getNum()==type1->getNum()&&
       pathTypeVector[i].getSiteType2()->getNum()==type2->getNum())
      return &pathTypeVector[i];
  }
  printf ("Cannot find!! path type %s %s\n",
	  type1->getName().c_str(),type2->getName().c_str());
  fatalError("Exit!");

  return NULL;
}


/*-----------------------------------------------*/
/*         乱数の発生                       */
/*-----------------------------------------------*/
double KineticMC::getRandomNumber(){
  return (double)rand()/(double)RAND_MAX;
}

void KineticMC::clearVectors(){
  vector<Site>::size_type i;
  for(i=0; i< siteVector.size(); i++){
    siteVector[i].clearVectors();
  }
  siteVector.clear();
  siteTypeVector.clear();
  pathTypeVector.clear();
  eventVector.clear();
}
