#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "ast.h"

/* Function checks all token-types in tree recursively, and make sure they match.
** Added VALID_TYPE and INVALID_TYPE to the eType enum to solve some problems.*/
eType typeCheck(t_tree current);

/*Function checks if specified 'id' is present in the symboltable*/
eType lookup(t_symtab *SymTab, char* id);

/*Function checks if the functions arguments matches it's parameters recursively*/
eType checkActuals(t_tree actuals, t_tree parameters);


#endif