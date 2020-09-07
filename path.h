#ifndef ARC_PATH_H
#define ARC_PATH_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

#include "site.h"

/*---- パスの種類 ----*/
class PathType{
 private:
  bool          twoSitesFlag;  /* 解離吸着,再結合脱離 */
  static    int numPathType;
  unsigned long numOccurrence;
  unsigned long lapOccurrence;
  int num;
  int      type1, type2;
  double   activEnergy, frequency, rate;
 public:
  PathType(int inType1, int inType2,
	   double inFrequency, double inActivEnergy,
	   double temperature);
  void      print();
  void      occur();
  int       getNum();
  double    getAverageOccu();
  int       getSiteType1();
  int       getSiteType2();
  double    getActivEnergy();
  double    getFrequency();
  double    getRate();
  unsigned long getNumOccurrence();
  bool      getTwoSitesFlag();
  void      twoSitesFlagOn();
  void      twoSitesFlagOff();
  void      printOccurrence();
  void      printOccurrence(ostream &stream);
  void      printLapOccurrence(ostream &stream);
};

#endif
