#ifndef ARC_PATH_H
#define ARC_PATH_H

/*---- パスの種類 ----*/
struct pathTypeInformation{
  int num;
  struct siteTypeInformation *type[2];
  double activEnergy, frequency, rate;
};

#endif
