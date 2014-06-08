
#include "printAST.h"
#include "nameAnalysis.h"
#include "typeCheck.h"
#include "offsetCalc.h"
#include <stdio.h>

void semanticAnalysis()
{
    if (nameAnalysis(treeRoot) == 0)
        return;
    if (typeCheck(treeRoot) == INVALID_TYPE)
        return;
    offsetCalc(treeRoot);
    

    printAST(treeRoot);
}