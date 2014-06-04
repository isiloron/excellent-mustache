
#include "printAST.h"
#include "nameAnalysis.h"


void semanticAnalysis()
{
    if (!nameAnalysis(treeRoot))
        return;

    printAST(treeRoot);
}