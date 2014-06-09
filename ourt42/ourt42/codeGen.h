#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

#include "emit.h"
#include "typeCheck.h"

/*Start function*/
void codeGen(char* lstname);

/*Function generates track42 code, by traversing the token-tree recurevley*/
void generateCode(t_tree current);

/*Function pops the correct amount of arguments recurevley, after a funcCall on the stack*/
void popActuals(t_tree actual);

#endif
