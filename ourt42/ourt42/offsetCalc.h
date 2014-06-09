#ifndef OFFSETCALC_H
#define OFFSETCALC_H

#include "ast.h"

/*Calculates the offset relative FP for formal and local
**variables. Also calculates the offset for the return value for a function.*/
void offsetCalc(t_tree current);

/*Calculates the return value offset for the function.*/
void retValOffsetCalc(t_tree function);

#endif