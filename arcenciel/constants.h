#ifndef ARC_CONSTANTS_H
#define ARC_CONSTANTS_H

/*************************************************/
/*         プログラム名 バージョン               */
/*************************************************/
const char * const PROGRAM_NAME = "ARC ver. 0.30";

const int LINE       = 1024;

/*************************************************/
/*         物理定数                              */
/*************************************************/

const double  AVOGADRO_CONST  = 6.0221367e23;  /* [/mol] */
const double  BOLTZMANN_CONST = 1.380658e-23; /* [J/K] */

const double  ELECTRONVOLT_TO_JOULE_PER_MOL = 9.648531e4;
/* [eV] -> [J/mol] */

const double  GAS_CONST = AVOGADRO_CONST*BOLTZMANN_CONST;
 /* [J/(mol*K)] */

#endif
