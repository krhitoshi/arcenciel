#ifndef ARC_PATH_H
#define ARC_PATH_H

#include "site.h"

/*---- パスの種類 ----*/
class PathType{
 private:
  static int numPathType;
  int num;
  SiteType *type1, *type2;
  double activEnergy, frequency, rate;
 public:
  PathType(SiteType *inType1, SiteType *inType2,
	   double inFrequency, double inActivEnergy,
	   double temperature);
  void      print();
  double    getRate();
  SiteType *getSiteType1();
  SiteType *getSiteType2();
};

#endif
