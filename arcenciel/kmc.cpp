#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"

/*************************************************/
/*         エラー出力、終了                      */
/*************************************************/
void fatalError(char *string)
{
  fprintf( stderr, "%s\n",string);
  exit(1);
}

/*************************************************/
/*         ファイルオープンエラー出力、終了      */
/*************************************************/
void fileOpenError(char *fileName)
{
  char errorString[LINE];
  
  sprintf(errorString, "Cannot open %s!!",fileName);
  fatalError(errorString);
}
