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

  vector<Site*>     neighborVector;
  vector<PathType*> pathTypeToNeighborVector;

  unsigned long num;              /* �ֹ� */
  SiteType     *type;             /* ���� */
  pos3D         pos;              /* ��ɸ */
  enumSiteState state;            /* ���� */
  int           numNeighbor;    /* ���ܤ��륵���Ȥο� */
 public:
  static int getNumSite();
  Site(struct position3D inPos, SiteType *inType);
  void clearVectors();
  unsigned long getNum();
  int  getNumNeighbor();
  int  getRealNumNeighbor();
  Site *getNeighbor(int index);
  void setPathTypeToNeighbor(int index, PathType *inPathType);
  PathType *getPathTypeToNeighbor(int index);
  SiteType      *getType();
  enumSiteState  getState();
  void           setState(enumSiteState value);
  void           addNeighbor(Site *inNeighbor, PathType *inPathType);
  vector<Site*>     *getNeighborVector();
  vector<PathType*> *getPathTypeToNeighborVector();
};

#endif
