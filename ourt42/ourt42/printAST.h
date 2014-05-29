#ifndef PRINTAST_H
#define PRINTAST_H

#include "ast.h"

void printAST(t_tree currentNode);
void printOperator(BINOP_KIND op);
void printType(eType type);

#endif