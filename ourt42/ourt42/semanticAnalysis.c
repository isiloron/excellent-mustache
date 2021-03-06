
#include "printAST.h"
#include "nameAnalysis.h"
#include "typeCheck.h"
#include "offsetCalc.h"
#include <stdio.h>

int semanticAnalysis()
{
    if (nameAnalysis(treeRoot) == 0)
        return 0;
    printf("Name analysis complete!\n");
    if (typeCheck(treeRoot) == INVALID_TYPE)
        return 0;
    printf("Type check complete!\n");
    offsetCalc(treeRoot);
    printf("Offset calculation complete!\n");
    

    //printAST(treeRoot);
}