#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;

#include "common.h"
#include "kineticMC.h"
#include "path.h"

/*************************************************/
/*         プログラム名 バージョン               */
/*************************************************/
const char * const PROGRAM_NAME = "ARC ver. 0.32";

KineticMC::KineticMC(){
  initialize();
}

bool KineticMC::mainProcedure(){
  try{
    siteTypeVector.push_back(SiteType("Ex"));
    loadInputFile();
    loadSite();
    printSiteInformation();
    loadRate();
    loadPath();
    createPathToExternalPhase();    
    printInputData();
    putParticles();
    mainLoop();
  }

  catch(string errorString){
    cout << "Error: " << errorString << endl;
    return false;
  }

  return true;
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

  systemTime    = 0.0;
  lapSystemTime = 0.0;
}

void KineticMC::loadInputFile(){
  FILE *fp;
  string fileName="input.kmc";
  char line[LINE], key[LINE];
  int  value, num, count;

  if( ( fp = fopen( fileName.c_str(), "r" ) ) == NULL ){
    string error = "Cannot Find! " + fileName;
    throw(error);
  }
  
  count = 1;
  while( fgets( line, LINE, fp ) != NULL ){
    if(line[0]=='#') continue;
    num = sscanf(line,"%s %d",key,&value);
    if(num<1){
      cout << "Empty Line [" << count <<"]:" << fileName << endl;
      count++;
      continue;
    }else if(num==1 || num > 2){
      string lineString = line;
      lineString.erase(lineString.size()-1,1);
      stringstream error;
      error << "Invalid Line ["  << count << "]: '" 
	  << lineString << "'";
      throw(string(error.str()));
    }
  
    
    if(!strcmp(key,"fileInterval")){
      fileOutputInterval = value;
    }else if(!strcmp(key,"displayInterval")){
      displayOutputInterval = value;
    }else if(!strcmp(key,"bulkParticle")){
      numParticle = value;
    }else if(!strcmp(key,"step")){
      numStep            = value;
    }else if(!strcmp(key,"temperature")){
      temperature        = value;
    }else if(!strcmp(key,"poisson")){
      if(value==0)
	timePoisson = false;
      else
	timePoisson = true;
    }else if(!strcmp(key,"seed")){
      seedType = value;
    }else{
      string error = "Unknown Keyword!: ";
      error += key;
      throw(error);
    }
    count++;
  }

  if(seedType==0){
    srand(time(NULL)); /* 乱数の種を現在の時間とする */
  }else{
    srand(seedType);   /* 0以外の値が入力されていればそれを使用 */  
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
  string fileName="rate.kmc";
  char line[LINE];
  char name1[LINE],name2[LINE];
  double distance;
  int  num;
  double frequency,activEnergy;
  SiteType *siteType1, *siteType2;

  if( ( fp = fopen( fileName.c_str(), "r" ) ) == NULL ){ 
    string error = "Cannot Find! " + fileName;
    throw(error);
    }

  while( fgets( line, LINE, fp ) != NULL ){
    num = sscanf(line, "%s %s %lf %lf %lf",
		 name1,name2,&frequency,&activEnergy,&distance);
    if ( num < 4 )
      cout << "Wrong Format in " << fileName << "!" << endl;
    cout << name1 << " " << name2 << endl;
    siteType1 = findSiteTypeNoAppend(name1);
    siteType2 = findSiteTypeNoAppend(name2);
    if(siteType1==NULL || siteType2==NULL) continue;
    pathTypeVector.push_back(PathType(siteType1,siteType2,
			    frequency,activEnergy,temperature));
    if(num == 5){
      pathTypeVector.back().twoSitesFlagOn();
      pathTypeVector.back().setTwoSitesDistance(distance);
    }

  }
  fclose(fp);

  vector<PathType>::size_type k;
  for(k=0;k<pathTypeVector.size();k++){
    pathTypeVector[k].print();
  }
}

/*-----------------------------------------------*/
/*         path情報の読み込み                    */
/*-----------------------------------------------*/
void KineticMC::loadPath(){
  FILE *fp;
  string fileName="path.kmc";
  char line[LINE];
  int count;

  if( ( fp = fopen( fileName.c_str(), "r" ) ) == NULL ){
    string error = "Cannot Find! " + fileName;
    throw(error);
  }

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
}


void KineticMC::createPathToExternalPhase(){
  vector<SiteType*> adsorptionTypeVector;
  vector<SiteType*> desorptionTypeVector;
  vector<SiteType*> adsorptionTypeVectorTwo;
  vector<SiteType*> desorptionTypeVectorTwo;

  vector<PathType>::size_type i;
  SiteType *exType = findSiteType("Ex");

  for(i=0;i<pathTypeVector.size();i++){
    if(pathTypeVector[i].getSiteType1()==exType){
      if(pathTypeVector[i].getTwoSitesFlag()){
	adsorptionTypeVectorTwo
	  .push_back(pathTypeVector[i].getSiteType2());
      }else{
	adsorptionTypeVector
	  .push_back(pathTypeVector[i].getSiteType2());
      }
    }else if(pathTypeVector[i].getSiteType2()==exType){
      if(pathTypeVector[i].getTwoSitesFlag()){
	desorptionTypeVectorTwo
	  .push_back(pathTypeVector[i].getSiteType1());
      }else{
	desorptionTypeVector
	  .push_back(pathTypeVector[i].getSiteType1());
      }
    }
  }
  {
    vector<Site>::iterator iter;
    iter = siteVector.begin();
    while(iter!=siteVector.end()){
      vector<SiteType*>::iterator iter2;
      iter2 = adsorptionTypeVector.begin();
      while(iter2!=adsorptionTypeVector.end()){
	if(iter->getType()==*iter2){
	  adsorptionSiteVector.push_back(&(*iter));	
	}
	iter2++;
      }
      iter2 = desorptionTypeVector.begin();
      while(iter2!=desorptionTypeVector.end()){
	if(iter->getType()==*iter2){
	  desorptionSiteVector.push_back(&(*iter));
	}
	iter2++;
      }
      iter++;
    }
  }

  adsorptionTypeVector.clear();
  desorptionTypeVector.clear();
  cout << "Adsorption Sites " << adsorptionSiteVector.size() << endl;
  cout << "Desorption Sites " << desorptionSiteVector.size() << endl;

  if(!adsorptionTypeVectorTwo.empty() ||
     !desorptionTypeVectorTwo.empty()){
    vector<Site>::iterator iter,iter2;
    vector<SiteType*>::iterator type;
    vector<Site*> *neighbors;
    vector<Site*>::iterator neighborIter;

    iter = siteVector.begin();
    while(iter!=siteVector.end()){
      iter2 = iter+1;
      while(iter2!=siteVector.end()){
	if(iter->getType() == iter2->getType()){
	  type = adsorptionTypeVectorTwo.begin();
	  while(type!=adsorptionTypeVectorTwo.end()){
	    if(iter->getType() == *type){
	      neighbors = iter->getNeighborVector();
	      neighborIter = neighbors->begin();
	      while(neighborIter != neighbors->end()){
		if(*neighborIter == &(*iter2)){
		  dissosiativeAdsorptionSiteVector
		    .push_back(SitePair(&(*iter),&(*iter2)));
		  break;
		}
		neighborIter++;
	      }
	      break;
	    }
	    type++;
	  }

	  type = desorptionTypeVectorTwo.begin();
	  while(type!=desorptionTypeVectorTwo.end()){
	    if(iter->getType() == *type){
	      neighbors = iter->getNeighborVector();
	      neighborIter = neighbors->begin();
	      while(neighborIter != neighbors->end()){
		if(*neighborIter == &(*iter2)){
		  recombinativeDesorptionSiteVector
		    .push_back(SitePair(&(*iter),&(*iter2)));
		  break;
		}
		neighborIter++;
	      }
	      break;
	    }
	    type++;
	  }
	}
	iter2++;
      }
      iter++;
    }
  }
  cout << "Dissociative  Adsorption Sites " 
       << dissosiativeAdsorptionSiteVector.size() << endl;
  cout << "Recombinative Desorption Sites "
       << recombinativeDesorptionSiteVector.size() << endl;

}

void KineticMC::loadSite(){
  FILE *fp;
  string fileName="site.kmc";
  char line[LINE];
  int count;

  cout << "loading " << fileName << "..." << endl;
  loadSiteType();
  if( ( fp = fopen( fileName.c_str(), "r" ) ) == NULL ) {
    string error = "Cannot Find! " + fileName;
    throw(error);
  }

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
  string fileName="site.kmc";
  char line[LINE];
  int count;
  char name[LINE];
  int num;
  SiteType *siteType;
  
  if( ( fp = fopen( fileName.c_str(), "r" ) ) == NULL ){
    string error = "Cannot Find! " + fileName;
    throw(error);
  }

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
      particleVector.push_back(&siteVector[random]);
    }else{
      i--;
    }
  }

  /*  vector<Site>::iterator iter;
  iter = siteVector.begin();
  while(iter!=siteVector.end()){
    if((iter->getType())->getName()==string("To"))
      particleVector.push_back(&(*iter));
    iter++;
    }*/

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
    throw(string("Cannot Open! out.kmc"));
  if( ( fp_time = fopen( "time.kmc", "w" )) == NULL ) 
    throw(string("Cannot Open! time.kmc"));

  ofstream occurrenceStream("occurrence.kmc");
  if(!occurrenceStream) {
    throw(string("Cannot Open! occurrence.kmc"));
  }

  cout << "############## Start ##############\n";

  printIntervalOutput(0,fp_out, fp_time);
  print10IntervalsOutput(0,occurrenceStream);

  /*---- ループスタート ----*/
  cout << "  STEP      TIME     LAPTIME    PARTICLES\n";
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

    updateSystemTime();
    /*    printf ("%u th event!! Time: %e\n",i,systemTime);*/
    eventVector[i].occur();
    if(eventVector[i].getEventType()==Event::DESORPTION){
       vector<Particle>::iterator iter;
       iter = particleVector.begin();
       while(iter!=particleVector.end()){
	 if( iter->getSite() == eventVector[i].getCurrentSite()){
	   particleVector.erase(iter);
	   break;
	 }
	 iter++;
       }
    }else if(eventVector[i].getEventType()==Event::ADSORPTION){
      particleVector.push_back(eventVector[i].getCurrentSite());
    }else if(eventVector[i].getEventType()
	     ==Event::DISSOCIATIVE_ADSORPTION){
      particleVector.push_back(eventVector[i].getCurrentSite());
      particleVector.push_back(eventVector[i].getCurrentSite2());
    }else if(eventVector[i].getEventType()
	     ==Event::RECOMBINATIVE_DESORPTION){
       vector<Particle>::iterator iter;
       int num = 0;
       iter = particleVector.begin();
       while(iter!=particleVector.end()){
	 if(num==2){
	   break;
	 }
	 if( iter->getSite() == eventVector[i].getCurrentSite()){
	   particleVector.erase(iter);
	   num++;
	 }else if( iter->getSite() == eventVector[i].getCurrentSite2()){
	   particleVector.erase(iter);
	   num++;
	 }else{
	   iter++;
	 }
       }
     }


    if(step!=0&&step%displayOutputInterval==0){
      printf ("%10d %10.5e %10.5e ",step,systemTime,lapSystemTime);
      lapSystemTime = 0.0;
      cout << particleVector.size() << endl;
      print10IntervalsOutput(step,occurrenceStream);
    }

    if(step!=0&&step%fileOutputInterval==0)
      printIntervalOutput(step,fp_out, fp_time);
  }
  /*---- ループエンド ----*/
  fclose(fp_out);
  fclose(fp_time);
  occurrenceStream.close();
  cout << "##############  End  ##############\n";
  //  printOccurrence();
}

/*-----------------------------------------------*/
/*         イベントのカウント                     */
/*-----------------------------------------------*/
void  KineticMC::countEvent(){
  vector<Particle>::size_type num;
  int i=0;
  int numNeighbor;
  Site *sitePointer;
  SiteType *exType = findSiteType("Ex");

  sumRate=0.0;
  double rate=0.0;

  for(num=0;num<particleVector.size();num++){
    i=0;
    sitePointer = particleVector[num].getSite();

    numNeighbor     = sitePointer->getRealNumNeighbor();
    vector<Site*> *neighbors = sitePointer->getNeighborVector();
    vector<Site*>::size_type count;
    vector<PathType*> *neighborPaths 
      = sitePointer->getPathTypeToNeighborVector();

    for(count=0;count<neighbors->size();count++){
      if((*neighbors)[count]->getState()!=Site::OCCUPY){
	rate = (*neighborPaths)[count]->getRate()/numNeighbor;
	eventVector.push_back(
	      Event(rate,&particleVector[num],
	    sitePointer,(*neighbors)[count],(*neighborPaths)[count]));

	sumRate += rate;
      }
      i++;
    }
  }

  {
    vector<Site*>::iterator iter;
    iter = desorptionSiteVector.begin();
    while(iter != desorptionSiteVector.end()){
      if((*iter)->getState()==Site::OCCUPY){
	PathType *path = findPathType((*iter)->getType(),exType);
	rate = path->getRate();
	eventVector.push_back(Event(rate, NULL,
				    *iter,path,Event::DESORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }

  {
    vector<Site*>::iterator iter;
    iter = adsorptionSiteVector.begin();
    while(iter != adsorptionSiteVector.end()){
      if((*iter)->getState()==Site::UNOCCUPY){
	PathType *path = findPathType(exType, (*iter)->getType());
	rate = path->getRate();
	eventVector.push_back(Event(rate, NULL,
				    *iter,path,Event::ADSORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }

  {
    vector<SitePair>::iterator iter;
    Site *site1, *site2;
    iter = recombinativeDesorptionSiteVector.begin();
    while(iter != recombinativeDesorptionSiteVector.end()){
      site1 = iter->getSite1();
      site2 = iter->getSite2();
      if(site1->getState()==Site::OCCUPY &&
	 site2->getState()==Site::OCCUPY){
	PathType *path = findPathType(site1->getType(),exType);
	rate = path->getRate();
	eventVector.push_back(Event(rate, NULL,NULL, site1,site2,
		    path,Event::RECOMBINATIVE_DESORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }

  {
    vector<SitePair>::iterator iter;
    Site *site1, *site2;
    iter = dissosiativeAdsorptionSiteVector.begin();
    while(iter != dissosiativeAdsorptionSiteVector.end()){
      site1 = iter->getSite1();
      site2 = iter->getSite2();
      if(site1->getState()==Site::UNOCCUPY &&
	 site2->getState()==Site::UNOCCUPY){
	PathType *path = findPathType(exType, site1->getType());
	rate = path->getRate();
	eventVector.push_back(Event(rate, NULL,NULL, site1,site2,
		    path,Event::DISSOCIATIVE_ADSORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }
}


/*-----------------------------------------------*/
/*         入力情報の出力                        */
/*-----------------------------------------------*/
void  KineticMC::printIntervalOutput(int step, FILE *fp_out, FILE *fp_time){
  fprintf (fp_time,"%10d %10.5e %10.5e\n",
	   step,systemTime,lapSystemTime);
  fflush(fp_time);

  vector<Particle>::size_type i;

  for(i=0;i<particleVector.size();i++){
    if(i==0) fprintf (fp_out,"%5d ",(int)(step/fileOutputInterval));
    else if((i%10)==0) 
      fprintf (fp_out,"\n%5d ",(int)(step/fileOutputInterval));
    
    fprintf (fp_out,"%5lu ",particleVector[i].getSite()->getNum());
  }
  fprintf (fp_out,"\n");
  fflush(fp_out);
}

void KineticMC::print10IntervalsOutput(int step, ostream &stream){
  vector<PathType>::size_type i;
  //  SiteType *exType = findSiteType("Ex");
  stream << step << " " << systemTime ;
  for(i=0;i<pathTypeVector.size();i++){
    //    if(pathTypeVector[i].getSiteType1()==exType||
    // pathTypeVector[i].getSiteType2()==exType)
    //	  pathTypeVector[i].printLapOccurrence(cout);
    if(step==0){
      stream << " " << pathTypeVector[i].getSiteNames();
    }else{
      stream << " " << pathTypeVector[i].getNumOccurrence();
    }
  }
  stream << endl;
}

/*-----------------------------------------------*/
/*         site数の読み込み                      */
/*-----------------------------------------------*/
void  KineticMC::loadNumSite( const char *line){
  int num;
  unsigned long dummy;
  num = sscanf( line, "%lu",&dummy);
  if ( num != 1 ) 
    throw(string("Not Find the number of sites!"));

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
    throw(string(
	 "The number of cell parameters is wrong!\nIt should be 6!"));

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
    throw(string("Not Find the number of paths!"));
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

SiteType* KineticMC::findSiteTypeNoAppend(char *name){
   int i;
  for(i=0;i<SiteType::getNumSiteType();i++){
    if(string(name) == siteTypeVector[i].getName()) 
      return &siteTypeVector[i];
  }
  cout << "Cannot find the site type: " << name << endl;
  return NULL;
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

  string error = "Cannot find!! path type "
    + type1->getName() + " " + type2->getName();

  throw(error);

  return NULL;
}


/*-----------------------------------------------*/
/*         乱数の発生                       */
/*-----------------------------------------------*/
double KineticMC::getRandomNumber(){
  return (double)rand()/(double)RAND_MAX;
}

void KineticMC::updateSystemTime(){
  double add;
  if(timePoisson){
    add = -log(getRandomNumber())/sumRate;
  }else{
    add = 1.0/sumRate;
  }

  systemTime += add;
  lapSystemTime += add;
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

void KineticMC::printOccurrence(){
  ofstream out("occurrence.kmc");
  if(!out) {
    cout << "Cannot Open occurrence.kmc";
    return;
  }
  vector<PathType>::size_type i;
  for(i=0;i< pathTypeVector.size();i++){
    pathTypeVector[i].printOccurrence(out);
    pathTypeVector[i].printOccurrence(cout);
  }
  out.close();
}
