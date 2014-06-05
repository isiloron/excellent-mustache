#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "ast.h"

eType typeCheck(t_tree current);
eType lookup(t_symtab *SymTab, char* id);
eType checkActuals(t_tree actuals, t_tree parameters);


#endif