
#include <stdio.h>
#include "ast.h"

t_symtab *varSymTab;
int formalOffset;
int localOffset;

void offsetCalc(t_tree current)
{
    if (current == NULL)
        return;

    switch (current->Kind)
    {
    case kProgram:
        offsetCalc(current->Node.Program.Functions);
        return;
    case kFunction:
        varSymTab = current->Node.Function.SymTab;
        formalOffset = -2;
        localOffset = 1;
        offsetCalc(current->Node.Function.Variables);
        offsetCalc(current->Node.Function.Next);
        return;
    case kVariable:
        switch (current->Node.Variable.VarKind)
        {
        case kFormal:
            switch (current->Node.Variable.Type)
            {
            case BOOL:
            case INT:
                formalOffset -= INT_SIZE;
                offsetCalc(current->Node.Variable.Next);
                formalOffset += INT_SIZE;
                ((SymTabData*)symtab_get(varSymTab, current->Node.Variable.Name))->Offset = formalOffset;
                return;
            case STRING:
                formalOffset -= STRING_SIZE;
                offsetCalc(current->Node.Variable.Next);
                formalOffset += STRING_SIZE;
                ((SymTabData*)symtab_get(varSymTab, current->Node.Variable.Name))->Offset = formalOffset;
                return;
            default:
                return;
            }
        case kLocal:
            ((SymTabData*)symtab_get(varSymTab, current->Node.Variable.Name))->Offset = localOffset;
            switch (current->Node.Variable.Type)
            {
            case BOOL:
            case INT:
                localOffset += INT_SIZE;
                break;
            case STRING:
                localOffset += STRING_SIZE;
                break;
            default:
                break;
            }
            offsetCalc(current->Node.Variable.Next);
            return;
        default:
            return;
        }
    default:
        return;
    }
}