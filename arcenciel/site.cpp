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

