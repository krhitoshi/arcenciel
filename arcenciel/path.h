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
  double        twoSitesDistance;
  static    int numPathType;
  unsigned long numOccurrence;
  unsigned long lapOccurrence;
  int num;
  SiteType *type1, *type2;
  double   activEnergy, frequency, rate;
 public:
  PathType(SiteType *inType1, SiteType *inType2,
	   double inFrequency, double inActivEnergy,
	   double temperature);
  void      print();
  void      occur();
  double    getRate();
  double    getAverageOccu();
  SiteType *getSiteType1();
  SiteType *getSiteType2();
  string    getSiteNames();
  unsigned long getNumOccurrence();
  bool      getTwoSitesFlag();
  void      twoSitesFlagOn();
  void      twoSitesFlagOff();
  void      setTwoSitesDistance(double inDistance);
  double    getTwoSitesDistance();
  void      printOccurrence();
  void      printOccurrence(ostream &stream);
  void      printLapOccurrence(ostream &stream);
};

#endif
