#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"

/*************************************************/
/*         ���顼���ϡ���λ                      */
/*************************************************/
void fatalError(char *string)
{
  fprintf( stderr, "%s\n",string);
  exit(1);
}

/*************************************************/
/*         �ե����륪���ץ󥨥顼���ϡ���λ      */
/*************************************************/
void fileOpenError(char *fileName)
{
  char errorString[LINE];
  
  sprintf(errorString, "Cannot open %s!!",fileName);
  fatalError(errorString);
}
