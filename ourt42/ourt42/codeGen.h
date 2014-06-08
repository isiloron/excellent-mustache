#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

#include "emit.h"
#include "typeCheck.h"

void codeGen(char* lstname);
void generateCode(t_tree current);
void popActuals(t_tree actual);

#endif
