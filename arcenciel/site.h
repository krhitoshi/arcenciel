#ifndef ARC_SITE_H
#define ARC_SITE_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "common.h"

/*---- サイトの種類 ----*/
class SiteType{
 private:
  static int numSiteType;     /* サイトの種類の総数 */
  int    num;                     /* 番号 */
  string name;                 /* 名前 */
 public:
  static int getNumSiteType(); /* サイトの種類の総数 */
  SiteType(string inName);
  int    getNum();
  void   setName(string value);
  string getName();
};

class PathType;

/*---- サイト情報 ----*/
class Site{
 public:
  /*---- サイトの状態 ----*/
  enum enumSiteState {UNOCCUPY, OCCUPY};
 private:
  static  int numSite;            /* サイトの総数 */

  vector<Site*>     neighborVector;
  vector<PathType*> pathTypeToNeighborVector;

  unsigned long num;              /* 番号 */
  SiteType     *type;             /* 種類 */
  pos3D         pos;              /* 座標 */
  enumSiteState state;            /* 状態 */
  int           numNeighbor;    /* 隣接するサイトの数 */
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
