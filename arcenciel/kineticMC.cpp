#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "common.h"
#include "kineticMC.h"

KineticMC::KineticMC(){
  initialize();
}

void KineticMC::initialize(){
  fileOutputInterval    = 50;
  displayOutputInterval = 10000;
  numStep               = 200000;
}

void KineticMC::loadInputFile(){
  FILE *fp;
  char fileName[]="input.kmc";
  char line[LINE], key[LINE];
  int  value;

  if( ( fp = fopen( fileName, "r" ) ) == NULL ) 
    fileOpenError(fileName);

  while( fgets( line, LINE, fp ) != NULL ){
    sscanf(line,"%s %d",key,&value);
    /*    printf ("Key: %s Value: %d\n",key,value);*/
    if(!strcmp(key,"fileInterval"))        
      fileOutputInterval = value;
    else if(!strcmp(key,"displayInterval")) 
      displayOutputInterval = value;
    else if(!strcmp(key,"step"))        numStep            = value;
    else if(!strcmp(key,"temperature")) temperature        = value;
    else
      cout << "Unknown Keyword!! :" << key << endl;
  }
  fclose(fp);
}

/*-----------------------------------------------*/
/*         入力情報の出力                        */
/*-----------------------------------------------*/
void KineticMC::printInputData(){
  cout << "Step                    : " << numStep << endl;
  cout << "Temperature             : " << temperature << endl;
  cout << "File    Output Interval : " << fileOutputInterval << endl;
  cout << "Display Output Interval : " << displayOutputInterval << endl;
}

