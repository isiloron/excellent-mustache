#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "ast.h"

/*Function checks all token-types in tree recurevley, and make sure they match eachother (VOID, BOOL, INT, STRING, INVALID_TYPE, VALID_TYPE)*/
eType typeCheck(t_tree current);

/*Function checks if specified 'id' is present in the symboltable recurevley*/
eType lookup(t_symtab *SymTab, char* id);

/*Function checks if the functions arguments matches it's parameters recurevley*/
eType checkActuals(t_tree actuals, t_tree parameters);


#endif