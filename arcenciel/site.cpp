#include "site.h"

int SiteType::numSiteType;

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

int Site::numSite;

Site::Site(struct position3D inPos, SiteType *inType){
  num  = numSite;
  pos  = inPos;
  type = inType;
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

int Site::getRealNumNeighbor(){
  int num=0;
  vector<Site*>::size_type i;
  for(i=0;i<neighborVector.size();i++){
    if(neighborVector[i]->getState()==Site::UNOCCUPY) num++;
  }
  return num;
}

Site *Site::getNeighbor(int index){
  return neighborVector[index];
}

PathType *Site::getPathTypeToNeighbor(int index){
  return pathTypeToNeighborVector[index];
}

void Site::setPathTypeToNeighbor(int index,PathType *inPathType){
  pathTypeToNeighborVector[index] = inPathType;
}

SiteType *Site::getType(){
  return type;
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


void Site::addNeighbor(Site *inNeighbor, PathType *inPathType){
  neighborVector.push_back(inNeighbor);
  pathTypeToNeighborVector.push_back(inPathType);
  numNeighbor++;
}

void Site::clearVectors(){
  neighborVector.clear();
  pathTypeToNeighborVector.clear();
}

vector<Site*> *Site::getNeighborVector(){
  return &neighborVector;
}

vector<PathType*> *Site::getPathTypeToNeighborVector(){
  return &pathTypeToNeighborVector;
}
