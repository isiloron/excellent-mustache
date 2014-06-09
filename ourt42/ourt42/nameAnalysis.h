#ifndef NAMEANALYSIS_H
#define NAMEANALYSIS_H

/* Function checks all id's, names and function-names recursively,
** and make sure they are declared before they are used in a scope.*/
int nameAnalysis(t_tree current);

#endif