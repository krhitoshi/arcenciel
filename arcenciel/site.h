#ifndef ARC_SITE_H
#define ARC_SITE_H
#include <iostream>
#include <string>
using namespace std;

#include "common.h"

/*---- サイトの種類 ----*/
class SiteType{
 private:
  static  int numSiteType;     /* サイトの種類の総数 */
  int num;
  string name;
 public:
  static int getNumSiteType(); /* サイトの種類の総数 */
  SiteType(string inName);
  int    getNum();
  void   setName(string value);
  string getName();
};


/*---- サイトの状態 ----*/
enum siteState {  UNOCCUPY, OCCUPY};

/*---- サイト情報 ----*/
struct siteInformation{
  unsigned long num;                /* 番号 */
  SiteType *type; /* 種類 */
  pos3D pos;                        /* 座標 */
  enum siteState state;             /* 状態 */
  int numNeighbor;                  /* 隣接するサイトの数 */
  struct siteInformation **neighbor; /* 隣接するサイトのポインタの配列 */
  struct pathTypeInformation **pathTypeToNeighbor;
};

#endif
