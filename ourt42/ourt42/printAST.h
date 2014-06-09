#ifndef PRINTAST_H
#define PRINTAST_H

#include "ast.h"

/*A very rudimentary set of functions which print the syntax tree to the console for debugging purposes.*/
void printAST(t_tree currentNode);
void printOperator(BINOP_KIND op);
void printType(eType type);

#endif