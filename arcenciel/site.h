#ifndef ARC_SITE_H
#define ARC_SITE_H
#include <iostream>
#include <string>
using namespace std;

#include "common.h"

/*---- �����Ȥμ��� ----*/
class SiteType{
 private:
  static  int numSiteType;     /* �����Ȥμ������� */
  int num;
  string name;
 public:
  static int getNumSiteType(); /* �����Ȥμ������� */
  SiteType(string inName);
  int    getNum();
  void   setName(string value);
  string getName();
};


/*---- �����Ȥξ��� ----*/
enum siteState {  UNOCCUPY, OCCUPY};

/*---- �����Ⱦ��� ----*/
struct siteInformation{
  unsigned long num;                /* �ֹ� */
  SiteType *type; /* ���� */
  pos3D pos;                        /* ��ɸ */
  enum siteState state;             /* ���� */
  int numNeighbor;                  /* ���ܤ��륵���Ȥο� */
  struct siteInformation **neighbor; /* ���ܤ��륵���ȤΥݥ��󥿤����� */
  struct pathTypeInformation **pathTypeToNeighbor;
};

#endif
