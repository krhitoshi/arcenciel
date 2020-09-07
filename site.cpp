#include "site.h"


/*---- static 変数 ----*/
int SiteType::numSiteType;     /* サイトの種類の総数 */
int Site::numSite;             /* サイトの総数 */

SiteType::SiteType(string inName){
  num = numSiteType;
  name = inName;
  numSiteType++;
}

int SiteType::getNumSiteType(){
  return numSiteType;
}

int SiteType::getNum(){
  return num;
}

string SiteType::getName(){
  return name;
}

void   SiteType::setName(string value){
  name = value;
}

Site::Site(struct position3D inPos, int inType){
  num  = numSite;
  pos  = inPos;
  siteType = inType;
  state = Site::UNOCCUPY;
  numNeighbor = 0;
  numSite++;
}

int Site::getNumSite(){
  return numSite;
}

int Site::getNumNeighbor(){
  return numNeighbor;
}

unsigned long Site::getNeighbor(int index){
  return neighborVector[index];
}

int Site::getPathTypeToNeighbor(int index){
  return pathTypeToNeighborVector[index];
}

void Site::setPathTypeToNeighbor(int index,int inPathType){
  pathTypeToNeighborVector[index] = inPathType;
}

int Site::getSiteType(){
  return siteType;
}

void Site::setState(enumSiteState value){
  state = value;
}

Site::enumSiteState Site::getState(){
  return state;
}

unsigned long Site::getNum(){
  return num;
}


void Site::addNeighbor(unsigned long inNeighbor, int inPathType){
  neighborVector.push_back(inNeighbor);
  pathTypeToNeighborVector.push_back(inPathType);
  numNeighbor++;
}

void Site::clear(){
  neighborVector.clear();
  pathTypeToNeighborVector.clear();
}

vector<unsigned long> *Site::getNeighborVector(){
  return &neighborVector;
}

vector<int> *Site::getPathTypeToNeighborVector(){
  return &pathTypeToNeighborVector;
}
