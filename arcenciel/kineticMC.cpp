#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

#include "common.h"
#include "kineticMC.h"
#include "path.h"

/*************************************************/
/*         プログラム名 バージョン               */
/*************************************************/
const char * const PROGRAM_NAME = "ARC ver. 0.37";
const char * const COPYRIGHT_STRING = 
"Copyright (C) 2002-2004 Hitoshi Kurokawa";

/*-----------------------------------------------*/
/*         コンストラクタ                        */
/*-----------------------------------------------*/
KineticMC::KineticMC(){
  initialize(); /* 計算パラメータの初期化 */
}

/*-----------------------------------------------*/
/*         メイン                                */
/*-----------------------------------------------*/
bool KineticMC::mainProcedure(){
  try{
    /* 特殊サイト種 Exの追加
       気相(Ex)からのサイトへの粒子吸着を表すパスの設定に使用 */
    siteTypeVector.push_back(SiteType("Ex"));

    loadInputFile();        /* 計算条件の読み込み */

    loadSite();             /* サイト情報の読み込み */
    printSiteInformation(); /* サイト情報の出力*/

    loadRate();             /* rate情報の読み込み */
    printRateInformation(); /* rate情報の出力 */

    loadPath();             /* パス情報の読み込み */

    /* rate情報において気相(Ex)とパスが設定されている場合に
       新たにExとのパス情報を作成する */
    createPathToExternalPhase();    

    loadParticle();         /* 粒子初期配置の読み込みと配置 */

    printInputData();       /* 入力データの出力 */

    mainLoop();             /* キネティックモンテカルロ計算開始 */
    clear();
  }

  /* エラー処理 */
  catch(string errorString){
    cout << "Error: " << errorString << endl;
    clear();
    return false; 
  }

  return true;
}

/*-----------------------------------------------*/
/*         プログラム名の表示                    */
/*-----------------------------------------------*/
void KineticMC::printProgramName(){
  if(silentFlag) return;
  cout << PROGRAM_NAME     << endl;
  cout << COPYRIGHT_STRING << endl;
}

/*-----------------------------------------------*/
/*         パラメータの初期化                    */
/*-----------------------------------------------*/
void KineticMC::initialize(){
  seedType              = 0;
  timePoisson           = true;
  silentFlag            = false;
  fileOutputInterval    = 50;
  displayOutputInterval = 10000;
  numStep               = 200000;

  temperature           = 300.0;

  systemTime    = 0.0;
  lapSystemTime = 0.0;

  /* 入力ファイル名 */
  siteFileName     = "site.kmc";
  pathFileName     = "path.kmc";
  rateFileName     = "rate.kmc";
  particleFileName = "particle.kmc";
}

/*-----------------------------------------------*/
/*         標準出力の設定                        */
/*-----------------------------------------------*/
void KineticMC::silentFlagOn(){
  silentFlag = true;
}

void KineticMC::silentFlagOff(){
  silentFlag = false;
}

/*-----------------------------------------------*/
/*         計算条件の読み込み input.kmc          */
/*-----------------------------------------------*/
void KineticMC::loadInputFile(){
  FILE *fp;
  string fileName="input.kmc";
  char   line[LINE], key[LINE];
  string keyString,valueString;
  bool   keyFlag, intFlag, doubleFlag, charFlag; 
  int    countLine;
  int    intValue; 
  double doubleValue;
  char   charValue[LINE];

  if( ( fp = fopen( fileName.c_str(), "r" ) ) == NULL ){
    string error = "Cannot Find! " + fileName;
    throw(error);
  }
  
  countLine = 1;
  while( fgets( line, LINE, fp ) != NULL ){
    if(line[0]=='#') continue; /* コメントアウト処理 */
    keyFlag=intFlag=doubleFlag=charFlag=false;

    if(sscanf(line,"%s",key)==1) keyFlag=true;
    keyString = key;
    if(sscanf(line,"%*s %d" ,&intValue)   ==1) intFlag    = true;
    if(sscanf(line,"%*s %lf",&doubleValue)==1) doubleFlag = true;
    if(sscanf(line,"%*s %s" ,charValue)   ==1) charFlag   = true;
    valueString = charValue;

    if(!keyFlag){
      if(!silentFlag)
	cout << "Empty Line [" << countLine <<"]:"
	     << fileName << endl;
      countLine++;
      continue;
    }else if(!(intFlag || doubleFlag || charFlag)){
      string lineString = line;
      lineString.erase(lineString.size()-1,1);
      char error[LINE];
      sprintf(error,"Invalid Line [%d]: %s",countLine,lineString.c_str());
      throw(string(error));
    }
  
    
    if(keyString == "fileInterval"){
      fileOutputInterval = intValue;
    }else if(keyString == "displayInterval"){
      displayOutputInterval = intValue;
    }else if(keyString == "step"){
      numStep            = intValue;
    }else if(keyString == "temperature"){
      temperature        = doubleValue;
    }else if(keyString == "poisson"){
      if(intValue==0)
	timePoisson = false;
      else
	timePoisson = true;
    }else if(keyString == "seed"){
      seedType = intValue;
    }else if(keyString == "siteFile"){
      siteFileName = valueString;
    }else if(keyString == "pathFile"){
      pathFileName = valueString;
    }else if(keyString == "rateFile"){
      rateFileName = valueString;
    }else{
      string error = "Unknown Keyword!: ";
      error += key;
      throw(error);
    }
    countLine++;
  }

  if(seedType==0){
    srand(time(NULL)); /* 乱数の種を現在の時間とする */
  }else{
    srand(seedType);   /* 0以外の値ならばそれを乱数の種とする */  
  }

  fclose(fp);
}

/*-----------------------------------------------*/
/*         site情報の読み込み site.kmc           */
/*-----------------------------------------------*/
void KineticMC::loadSite(){
  FILE *fp;
  char line[LINE];
  int count;

  if( ( fp = fopen( siteFileName.c_str(), "r" ) ) == NULL ) {
    string error = "Cannot Find! " + siteFileName;
    throw(error);
  }

  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    if(line[0]=='#') continue; /* コメントアウト処理 */

    if( count == 1 )
      ;                         /* 一行目はスキップ(タイトル等) */
    else if( count == 2 )
      loadCellParameters(line); /* セルパラメータの読み込み */
    else
      loadCoordination(line);   /* サイト種, 座標の読み込み */
    count++;
  }
  fclose(fp);
}

/*-----------------------------------------------*/
/*         rate情報の読み込み rate.kmc           */
/*         path種, rate, 活性化エネルギー        */
/*-----------------------------------------------*/
void KineticMC::loadRate(){
  FILE *fp;
  char line[LINE];
  char name1[LINE],name2[LINE];
  double distance;
  int  num;
  double frequency,activEnergy;
  int siteType1, siteType2;

  if( ( fp = fopen( rateFileName.c_str(), "r" ) ) == NULL ){ 
    string error = "Cannot Find! " + rateFileName;
    throw(error);
    }

  while( fgets( line, LINE, fp ) != NULL ){
    if(line[0]=='#') continue; /* コメントアウト処理 */
    num = sscanf(line, "%s %s %lf %lf %lf",
		 name1,name2,&frequency,&activEnergy,&distance);

    if ( num < 4 && !silentFlag)
      cout << "Wrong Format in " << rateFileName << "!" << endl;

    siteType1 = findSiteTypeNoAppend(name1);
    siteType2 = findSiteTypeNoAppend(name2);

    if(siteType1==-1 || siteType2==-1) continue;
    pathTypeVector.push_back(PathType(siteType1,siteType2,
			    frequency,activEnergy,temperature));
    if(num == 5){
      pathTypeVector.back().twoSitesFlagOn();
    }

  }
  fclose(fp);
}

/*-----------------------------------------------*/
/*         path情報の読み込み path.kmc           */
/*-----------------------------------------------*/
void KineticMC::loadPath(){
  FILE *fp;
  char line[LINE];
  int count;

  if( ( fp = fopen( pathFileName.c_str(), "r" ) ) == NULL ){
    string error = "Cannot Find! " + pathFileName;
    throw(error);
  }

  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    if(line[0]=='#') continue; /* コメントアウト処理 */

    if( count == 1 )
      ;
    else
      loadPair(line);
    count++;
  }
  {
    vector<Site>::size_type i;
    int j;
    unsigned long neighbor;
    for(i=0;i<siteVector.size();i++){
      for(j=0;j<siteVector[i].getNumNeighbor();j++){
	neighbor = siteVector[i].getNeighbor(j);
	siteVector[i].setPathTypeToNeighbor(j,
	   findPathType(siteVector[i].getSiteType(), 
			siteVector[neighbor].getSiteType()));
      }
    }
  }
}

/*-----------------------------------------------*/
/*       粒子初期配置情報の読み込み particle.kmc */
/*-----------------------------------------------*/
void KineticMC::loadParticle(){
  FILE *fp;
  char line[LINE];
  int count;

  if( ( fp = fopen( particleFileName.c_str(), "r" ) ) == NULL ){
    string error = "Cannot Find! " + particleFileName;
    throw(error);
  }

  char   key[LINE];
  string keyString;
  unsigned long site[10];
  int    num, i;
  unsigned long j;
  count=1;
  while( fgets( line, LINE, fp ) != NULL ){
    if(line[0]=='#') continue; /* コメントアウト処理 */
    sscanf(line,"%s",key);
    keyString = key;

    if(keyString=="Random"){  /* ランダムに粒子を配置 */
      unsigned long numRandomParticle;
      unsigned long random;
      sscanf(line,"%*s %*s %lu",&numRandomParticle);
      for(j=0;j<numRandomParticle;j++){
	random = (unsigned long)
	  (getRandomNumber()*siteVector.size());
	if(siteVector[random].getState()==Site::UNOCCUPY){
	  particleVector.push_back(siteVector[random].getNum());
	  siteVector[random].setState(Site::OCCUPY);
	}else{
	  j--;
	}
      }
    }else{                   /* 特定サイトに粒子を配置 */
      num = sscanf(line,"%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
		   &site[0],&site[1],&site[2],&site[3],&site[4],
		   &site[5],&site[6],&site[7],&site[8],&site[9]);
      for(i=0; i<num; i++){
	if(siteVector[site[i]].getState()==Site::UNOCCUPY){
	  particleVector.push_back(siteVector[site[i]].getNum());
	  siteVector[site[i]].setState(Site::OCCUPY);
	}else{
	  char error[LINE];
	  sprintf(error,"Site[%lu] is already occupied!",site[i]); 
	  throw(string(error));
	}
      }
    }
    
    count++;
  }
}

/*-----------------------------------------------*/
/*         入力情報の出力                        */
/*-----------------------------------------------*/
void KineticMC::printInputData(){
  if(silentFlag) return;
  cout << "Step                     : " << numStep << endl;
  cout << "Temperature              : " << temperature << endl;
  cout << "File    Output Interval  : " << fileOutputInterval << endl;
  cout << "Display Output Interval  : " << displayOutputInterval << endl;
}

void KineticMC::createPathToExternalPhase(){
  vector<int> adsorptionTypeVector;
  vector<int> desorptionTypeVector;
  vector<int> adsorptionTypeVectorTwo;
  vector<int> desorptionTypeVectorTwo;

  vector<PathType>::size_type i;
  int exType = findSiteType("Ex");

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
      vector<int>::iterator iter2;
      iter2 = adsorptionTypeVector.begin();
      while(iter2!=adsorptionTypeVector.end()){
	if(iter->getSiteType()==*iter2){
	  adsorptionSiteVector.push_back(iter->getNum());	
	}
	iter2++;
      }
      iter2 = desorptionTypeVector.begin();
      while(iter2!=desorptionTypeVector.end()){
	if(iter->getSiteType()==*iter2){
	  desorptionSiteVector.push_back(iter->getNum());
	}
	iter2++;
      }
      iter++;
    }
  }

  adsorptionTypeVector.clear();
  desorptionTypeVector.clear();
  if(!silentFlag){
    cout << "Adsorption Sites " << adsorptionSiteVector.size() << endl;
    cout << "Desorption Sites " << desorptionSiteVector.size() << endl;
  }

  if(!adsorptionTypeVectorTwo.empty() ||
     !desorptionTypeVectorTwo.empty()){
    vector<Site>::iterator iter,iter2;
    vector<int>::iterator type;
    vector<unsigned long> *neighbors;
    vector<unsigned long>::iterator neighborIter;

    iter = siteVector.begin();
    while(iter!=siteVector.end()){
      iter2 = iter+1;
      while(iter2!=siteVector.end()){
	if(iter->getSiteType() == iter2->getSiteType()){
	  type = adsorptionTypeVectorTwo.begin();
	  while(type!=adsorptionTypeVectorTwo.end()){
	    if(iter->getSiteType() == *type){
	      neighbors = iter->getNeighborVector();
	      neighborIter = neighbors->begin();
	      while(neighborIter != neighbors->end()){
		if(*neighborIter == iter2->getNum()){
		  dissosiativeAdsorptionSiteVector
	    .push_back(SitePair(iter->getNum(),iter2->getNum()));
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
	    if(iter->getSiteType() == *type){
	      neighbors = iter->getNeighborVector();
	      neighborIter = neighbors->begin();
	      while(neighborIter != neighbors->end()){
		if(*neighborIter == iter2->getNum()){
		  recombinativeDesorptionSiteVector
	    .push_back(SitePair(iter->getNum(),iter2->getNum()));
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
  if(!silentFlag){
    cout << "Dissociative  Adsorption Sites " 
	 << dissosiativeAdsorptionSiteVector.size() << endl;
    cout << "Recombinative Desorption Sites "
	 << recombinativeDesorptionSiteVector.size() << endl;
  }
}

/*-----------------------------------------------*/
/*         メインループ                          */
/*-----------------------------------------------*/
void KineticMC::mainLoop(){
  int step;
  long double eventRandom;
  vector<Event>::size_type i;  
  FILE *fp_out, *fp_time;

  if( ( fp_out = fopen( "out.kmc", "w" )  ) == NULL ) 
    throw(string("Cannot Open! out.kmc"));
  if( ( fp_time = fopen( "time.kmc", "w" )) == NULL ) 
    throw(string("Cannot Open! time.kmc"));

  ofstream occurrenceStream("occurrence.kmc");
  if(!occurrenceStream) {
    throw(string("Cannot Open! occurrence.kmc"));
  }

  if(!silentFlag)
    cout << "############## Start ##############\n";

  printIntervalOutput(0,fp_out, fp_time);
  printOccurrence(0,occurrenceStream);

  /*---- ループスタート ----*/
  if(!silentFlag)
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
    
    eventOccur(i);


    if(step!=0&&step%displayOutputInterval==0){
      if(!silentFlag){
	printf ("%10d %10.5e %10.5e ",step,systemTime,lapSystemTime);
	cout << particleVector.size() << endl;
      }
      lapSystemTime = 0.0;
    }

    if(step!=0&&step%fileOutputInterval==0){
      printIntervalOutput(step,fp_out, fp_time);
      printOccurrence(step,occurrenceStream);
    }

  }
  /*---- ループエンド ----*/
  fclose(fp_out);
  fclose(fp_time);
  occurrenceStream.close();
  if(!silentFlag)
    cout << "##############  End  ##############\n";
}

/*-----------------------------------------------*/
/*         イベントのカウント                     */
/*-----------------------------------------------*/
void  KineticMC::countEvent(){
  vector<Particle>::size_type num;
  int i=0;
  int numNeighbor;
  unsigned long site;
  int exType = findSiteType("Ex");

  sumRate=0.0;
  double rate=0.0;

  for(num=0;num<particleVector.size();num++){
    i=0;
    site = particleVector[num].getSite();

    numNeighbor     = getRealNumNeighbor(site);
    vector<unsigned long> *neighbors = 
      siteVector[site].getNeighborVector();
    vector<unsigned long>::size_type count;
    vector<int> *neighborPaths 
      = siteVector[site].getPathTypeToNeighborVector();

    for(count=0;count<neighbors->size();count++){
      if(siteVector[(*neighbors)[count]].getState()!=Site::OCCUPY){
	int pathType=(*neighborPaths)[count];
	rate = pathTypeVector[pathType].getRate()/numNeighbor;
	eventVector.push_back(
	      Event(rate, site,(*neighbors)[count],pathType));

	sumRate += rate;
      }
      i++;
    }
  }

  {
    vector<unsigned long>::iterator iter;
    iter = desorptionSiteVector.begin();
    while(iter != desorptionSiteVector.end()){
      if(siteVector[*iter].getState()==Site::OCCUPY){
	int pathType = 
	  findPathType(siteVector[*iter].getSiteType(),exType);
	rate = pathTypeVector[pathType].getRate();
	eventVector.push_back(Event(rate, *iter,pathType,Event::DESORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }

  {
    vector<unsigned long>::iterator iter;
    iter = adsorptionSiteVector.begin();
    while(iter != adsorptionSiteVector.end()){
      if(siteVector[(*iter)].getState()==Site::UNOCCUPY){
	int pathType = findPathType(exType, 
			    siteVector[(*iter)].getSiteType());
	rate = pathTypeVector[pathType].getRate();
	eventVector.push_back(Event(rate,
				    *iter,pathType,Event::ADSORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }

  {
    vector<SitePair>::iterator iter;
    unsigned long site1, site2;
    iter = recombinativeDesorptionSiteVector.begin();
    while(iter != recombinativeDesorptionSiteVector.end()){
      site1 = iter->getSite1();
      site2 = iter->getSite2();
      if(siteVector[site1].getState()==Site::OCCUPY &&
	 siteVector[site2].getState()==Site::OCCUPY){
	int pathType = 
	  findPathType(siteVector[site1].getSiteType(),exType);
	rate = pathTypeVector[pathType].getRate();
	eventVector.push_back(Event(rate, site1,site2,
		    pathType,Event::RECOMBINATIVE_DESORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }

  {
    vector<SitePair>::iterator iter;
    unsigned long site1, site2;
    iter = dissosiativeAdsorptionSiteVector.begin();
    while(iter != dissosiativeAdsorptionSiteVector.end()){
      site1 = iter->getSite1();
      site2 = iter->getSite2();
      if(siteVector[site1].getState()==Site::UNOCCUPY &&
	 siteVector[site2].getState()==Site::UNOCCUPY){
	int pathType =
	  findPathType(exType, siteVector[site1].getSiteType());
	rate = pathTypeVector[pathType].getRate();
	eventVector.push_back(Event(rate, site1,site2,
		    pathType,Event::DISSOCIATIVE_ADSORPTION));
	sumRate += rate;
      }
      iter++;
    }
  }
}

/*-----------------------------------------------*/
/*         site情報の出力                        */
/*-----------------------------------------------*/
void KineticMC::printSiteInformation(){
  if(silentFlag) return;

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
/*         path情報の出力                        */
/*-----------------------------------------------*/
void KineticMC::printRateInformation(){
  if(silentFlag) return;

  cout.setf(ios::fixed);
  cout.precision(5);


  vector<PathType>::size_type k;
  for(k=0;k<pathTypeVector.size();k++){
    //    pathTypeVector[k].print();
    cout << "Path Type "
	 << " " << setw(3) << pathTypeVector[k].getNum() << ": "
	 << " " << setw(5) << 
      siteTypeVector[pathTypeVector[k].getSiteType1()].getName()
	 << " " << setw(5) << 
      siteTypeVector[pathTypeVector[k].getSiteType2()].getName()
	 << " " << setw(10) << pathTypeVector[k].getActivEnergy();
    cout.setf(ios::fixed|ios::scientific);
    cout << " " << setw(10) << pathTypeVector[k].getFrequency()
	 << " " << setw(10) << pathTypeVector[k].getRate() << endl;
    
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
    
    fprintf (fp_out,"%5lu ",particleVector[i].getSite());
  }
  fprintf (fp_out,"\n");
  fflush(fp_out);
}

void KineticMC::printOccurrence(int step, ostream &stream){
  vector<PathType>::size_type i;
  //  SiteType *exType = findSiteType("Ex");
  int  type1, type2;
  char name[LINE];
  stream << step << " " << systemTime << " " << particleVector.size() ;
  for(i=0;i<pathTypeVector.size();i++){
    //    if(pathTypeVector[i].getSiteType1()==exType||
    // pathTypeVector[i].getSiteType2()==exType)
    //	  pathTypeVector[i].printLapOccurrence(cout);
    type1 = pathTypeVector[i].getSiteType1();
    type2 = pathTypeVector[i].getSiteType2();
    if(step==0){
      sprintf(name," %s-%s",siteTypeVector[type1].getName().c_str()
	      ,siteTypeVector[type2].getName().c_str());
	stream << name;
    }else{
      stream << " " << pathTypeVector[i].getNumOccurrence();
    }
  }
  stream << endl;
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
  if ( num != 4 && !silentFlag)
    cout << "Wrong coordination!!" << endl;

  int foundSiteType = findSiteType(name);

  siteVector.push_back(Site(pos,foundSiteType));
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
  unsigned long site1, site2;
  int type1, type2;

  num = sscanf(line, 
      "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"
   ,&pair[0][0],&pair[0][1],&pair[1][0],&pair[1][1],&pair[2][0],&pair[2][1]
   ,&pair[3][0],&pair[3][1],&pair[4][0],&pair[4][1],&pair[5][0],&pair[5][1]
   ,&pair[6][0],&pair[6][1],&pair[7][0],&pair[7][1],&pair[8][0],&pair[8][1]
   ,&pair[9][0],&pair[9][1]);
      
  if(num%2!=0 && !silentFlag)
    cout << "Wrong path!!" << endl;
  numPair = (int)((double)num/2.0);
  for(i=0;i<numPair;i++){
    site1 = siteVector[pair[i][0]].getNum();
    site2 = siteVector[pair[i][1]].getNum();
    type1 = siteVector[pair[i][0]].getSiteType();
    type2 = siteVector[pair[i][1]].getSiteType();

    siteVector[pair[i][0]].addNeighbor(site2,findPathType(type1,type2));
    siteVector[pair[i][1]].addNeighbor(site1,findPathType(type2,type1));
  }
}


/*-----------------------------------------------*/
/*         site typeの追加                       */
/*-----------------------------------------------*/
int KineticMC::addSiteType(char *name){
  siteTypeVector.push_back(SiteType(name));

  /*  printf ("%d %s \n",siteType[siteTypeNum-1].num,
      siteType[siteTypeNum-1].name);*/
  return siteTypeVector.back().getNum();
}


/*-----------------------------------------------*/
/*         site typeの検索                       */
/*-----------------------------------------------*/
int KineticMC::findSiteType(char *name){
   int i;
  for(i=0;i<SiteType::getNumSiteType();i++){
    if(string(name) == siteTypeVector[i].getName()) 
      return siteTypeVector[i].getNum();
  }
  return addSiteType(name);  
}

int KineticMC::findSiteTypeNoAppend(char *name){
   int i;
  for(i=0;i<SiteType::getNumSiteType();i++){
    if(string(name) == siteTypeVector[i].getName()) 
      return siteTypeVector[i].getNum();
  }
  if(!silentFlag)
    cout << "Cannot find the site type: " << name << endl;
  return -1;
}


/*-----------------------------------------------*/
/*         pair typeの検索                       */
/*-----------------------------------------------*/
int  KineticMC::findPathType(int type1,int type2){
  vector<PathType>::size_type i;

  for(i=0;i<pathTypeVector.size();i++){
    if(pathTypeVector[i].getSiteType1()==type1&&
       pathTypeVector[i].getSiteType2()==type2)
      return pathTypeVector[i].getNum();
  }

  string error = "Cannot find!! path type "
    + siteTypeVector[type1].getName() + " " 
    + siteTypeVector[type2].getName();

  throw(error);

  return -1;
}


/*-----------------------------------------------*/
/*         乱数の発生                            */
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

void KineticMC::clear(){
  vector<Site>::size_type i;
  for(i=0; i< siteVector.size(); i++){
    siteVector[i].clearVectors();
  }
  siteVector.clear();
  siteTypeVector.clear();

  pathTypeVector.clear();

  eventVector.clear();

  adsorptionSiteVector.clear();
  desorptionSiteVector.clear();
  dissosiativeAdsorptionSiteVector.clear();
  recombinativeDesorptionSiteVector.clear();
}

int KineticMC::getRealNumNeighbor(int site){
  vector<unsigned long> *neighbors;
  vector<unsigned long>::size_type i;
  neighbors = siteVector[site].getNeighborVector();
  int num=0;
  for(i=0;i<neighbors->size();i++){
    if(siteVector[(*neighbors)[i]].getState()==Site::UNOCCUPY) num++;
  }
  return num;

}

void KineticMC::eventOccur(vector<Event>::size_type index){
  
  Event::enumEventType eventType = eventVector[index].getEventType();
  unsigned long currentSite = eventVector[index].getCurrentSite();
  unsigned long currentSite2 = eventVector[index].getCurrentSite2();
  unsigned long nextSite = eventVector[index].getNextSite();
  
  if(eventType==Event::DIFFUSION){
    siteVector[currentSite].setState(Site::UNOCCUPY);
    siteVector[nextSite].setState(Site::OCCUPY);
    vector<Particle>::iterator iter;
    iter = particleVector.begin();
    while(iter!=particleVector.end()){
      if( iter->getSite() == eventVector[index].getCurrentSite()){
	iter->setSite(nextSite);
	break;
      }
      iter++;
    }
  }else if(eventType==Event::DESORPTION){
    siteVector[currentSite].setState(Site::UNOCCUPY);


  }else if(eventType==Event::ADSORPTION){
    //    currentSite->setState(Site::OCCUPY);
    /* 粒子を生成する際にOCCUPYにしているのでここでは設定不要 */


  }else if(eventType==Event::DISSOCIATIVE_ADSORPTION){
    /* 粒子を生成する際にOCCUPYにしているのでここでは設定不要 */


  }else if(eventType==Event::RECOMBINATIVE_DESORPTION){
    siteVector[currentSite].setState(Site::UNOCCUPY);
    siteVector[currentSite2].setState(Site::UNOCCUPY);
  }

  pathTypeVector[eventVector[index].getPathType()].occur();
  
  if(eventVector[index].getEventType()==Event::DESORPTION){
    vector<Particle>::iterator iter;
    iter = particleVector.begin();
    while(iter!=particleVector.end()){
      if( iter->getSite() == eventVector[index].getCurrentSite()){
	particleVector.erase(iter);
	break;
      }
      iter++;
    }
  }else if(eventVector[index].getEventType()==Event::ADSORPTION){
    particleVector.push_back(eventVector[index].getCurrentSite());
    siteVector[eventVector[index].getCurrentSite()]
      .setState(Site::OCCUPY);
  }else if(eventVector[index].getEventType()
	   ==Event::DISSOCIATIVE_ADSORPTION){
    particleVector.push_back(eventVector[index].getCurrentSite());
    siteVector[eventVector[index].getCurrentSite()]
      .setState(Site::OCCUPY);
    
    particleVector.push_back(eventVector[index].getCurrentSite2());
    siteVector[eventVector[index].getCurrentSite2()]
      .setState(Site::OCCUPY);
  }else if(eventVector[index].getEventType()
	   ==Event::RECOMBINATIVE_DESORPTION){
    vector<Particle>::iterator iter;
    int num = 0;
    iter = particleVector.begin();
    while(iter!=particleVector.end()){
      if(num==2){
	break;
      }
      if( iter->getSite() == eventVector[index].getCurrentSite()){
	particleVector.erase(iter);
	num++;
      }else if( iter->getSite() == eventVector[index].getCurrentSite2()){
	particleVector.erase(iter);
	num++;
      }else{
	iter++;
      }
    }
  }
}
