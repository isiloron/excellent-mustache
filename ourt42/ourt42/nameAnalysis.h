#ifndef NAMEANALYSIS_H
#define NAMEANALYSIS_H

#include "ast.h"

int nameAnalysis(t_tree current, t_symtab *funcSymTab, t_symtab *varSymTab);

#endif