#include "path.h"
#include "math.h"

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

int PathType::numPathType;

PathType::PathType(SiteType *inType1, SiteType *inType2,
		   double inFrequency, double inActivEnergy,
		   double temperature){
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

string PathType::getSiteNames(){
  string temp;
  temp  = type1->getName();
  temp += "-";
  temp += type2->getName();
  return temp;
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

void PathType::occur(){
  numOccurrence++;  
  lapOccurrence++;
}

unsigned long PathType::getNumOccurrence(){
  return numOccurrence;
}

void PathType::printOccurrence(){
  cout << setw(5) << type1->getName()
       << " - "
       << setw(5) << type2->getName()
       << ":" << numOccurrence << endl;
}

void PathType::printOccurrence(ostream &stream){
  stream << setw(5) << type1->getName()
	 << " - "
	 << setw(5) << type2->getName()
	 << ":" << numOccurrence << endl;
}

void PathType::printLapOccurrence(ostream &stream){
  stream << setw(5) << type1->getName()
	 << " - "
	 << setw(5) << type2->getName()
	 << ":" << lapOccurrence << endl;
  lapOccurrence = 0;
}
