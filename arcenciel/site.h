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

  vector<unsigned long>     neighborVector;
  vector<int> pathTypeToNeighborVector;

  unsigned long num;              /* 番号 */
  int           siteType;         /* 種類 */
  pos3D         pos;              /* 座標 */
  enumSiteState state;            /* 状態 */
  int           numNeighbor;    /* 隣接するサイトの数 */
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
