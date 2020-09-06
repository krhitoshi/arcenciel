#include "path.h"
#include "math.h"

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

int PathType::numPathType;

PathType::PathType(int inType1, int inType2,
		   double inFrequency, double inActivEnergy,
		   double temperature){
  twoSitesFlag     = false;
  numOccurrence = 0;
  lapOccurrence = 0;
  num = numPathType;
  type1       = inType1;
  type2       = inType2;
  frequency   = inFrequency;
  activEnergy = inActivEnergy;
  rate = frequency * 
    exp(-activEnergy*ELECTRONVOLT_TO_JOULE_PER_MOL/
	(GAS_CONST*temperature));
  numPathType++;
}

int PathType::getNum(){
  return num;
}

void PathType::print(){
  /*  cout.setf(ios::fixed);
  cout.precision(5);

  cout << "Path Type "
       << " " << setw(3) << num << ": "
       << " " << setw(5) << type1->getName()
       << " " << setw(5) << type2->getName()
       << " " << setw(10) << activEnergy;
  cout.setf(ios::fixed|ios::scientific);
  cout << " " << setw(10) << frequency
  << " " << setw(10) << rate << endl;*/
}

double PathType::getRate(){
  return rate;
}

double PathType::getActivEnergy(){
  return activEnergy;
}

double PathType::getFrequency(){
  return frequency;
}

int PathType::getSiteType1(){
  return type1;
}

int PathType::getSiteType2(){
  return type2;
}

void PathType::occur(){
  numOccurrence++;  
  lapOccurrence++;
}

bool PathType::getTwoSitesFlag(){
  return twoSitesFlag;
}

void PathType::twoSitesFlagOn(){
  twoSitesFlag = true;
}

void PathType::twoSitesFlagOff(){
  twoSitesFlag = false;
}

unsigned long PathType::getNumOccurrence(){
  return numOccurrence;
}
