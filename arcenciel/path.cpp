#include "path.h"
#include "math.h"

#include <iostream>
#include <iomanip>
using namespace std;

int PathType::numPathType;

PathType::PathType(SiteType *inType1, SiteType *inType2,
		   double inFrequency, double inActivEnergy,
		   double temperature){
  numOccurrence = 0;
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

void PathType::print(){
  cout.setf(ios::fixed);
  cout.precision(5);

  cout << "Path Type "
       << " " << setw(3) << num << ": "
       << " " << setw(5) << type1->getName()
       << " " << setw(5) << type2->getName()
       << " " << setw(10) << activEnergy;
  cout.setf(ios::fixed|ios::scientific);
  cout << " " << setw(10) << frequency
       << " " << setw(10) << rate << endl;
}

double PathType::getRate(){
  return rate;
}

SiteType *PathType::getSiteType1(){
  return type1;
}

SiteType *PathType::getSiteType2(){
  return type2;
}

unsigned long PathType::getNumOccurrence(){
  return numOccurrence;
}
