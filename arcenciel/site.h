#ifndef ARC_SITE_H
#define ARC_SITE_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "common.h"

/*---- �����Ȥμ��� ----*/
class SiteType{
 private:
  static int numSiteType;     /* �����Ȥμ������� */
  int    num;                     /* �ֹ� */
  string name;                 /* ̾�� */
 public:
  static int getNumSiteType(); /* �����Ȥμ������� */
  SiteType(string inName);
  int    getNum();
  void   setName(string value);
  string getName();
};

class PathType;

/*---- �����Ⱦ��� ----*/
class Site{
 public:
  /*---- �����Ȥξ��� ----*/
  enum enumSiteState {UNOCCUPY, OCCUPY};
 private:
  static  int numSite;            /* �����Ȥ���� */

  vector<unsigned long>     neighborVector;
  vector<int> pathTypeToNeighborVector;

  unsigned long num;              /* �ֹ� */
  int           siteType;         /* ���� */
  pos3D         pos;              /* ��ɸ */
  enumSiteState state;            /* ���� */
  int           numNeighbor;    /* ���ܤ��륵���Ȥο� */
 public:
  static int getNumSite();
  Site(struct position3D inPos, int inType);
  void clearVectors();
  unsigned long getNum();
  int  getNumNeighbor();
  unsigned long getNeighbor(int index);
  void setPathTypeToNeighbor(int index, int inPathType);
  int  getPathTypeToNeighbor(int index);
  int            getSiteType();
  enumSiteState  getState();
  void           setState(enumSiteState value);
  void           addNeighbor(unsigned long inNeighbor, int inPathType);
  vector<unsigned long>     *getNeighborVector();
  vector<int>   *getPathTypeToNeighborVector();
};

#endif
