
#include <stdio.h>
#include "ast.h"

t_symtab *varSymTab;
t_symtab *funcSymTab;
int formalOffset;
int localOffset;
int retValOffset;
t_tree iterator;

void offsetCalc(t_tree current)
{
    if (current == NULL)
        return;

    switch (current->Kind)
    {
    case kProgram:
        funcSymTab = current->Node.Program.SymTab;
        offsetCalc(current->Node.Program.Functions);        
        return;
    case kFunction:
        varSymTab = current->Node.Function.SymTab;
        formalOffset = 2;
        localOffset = 0;
        offsetCalc(current->Node.Function.Variables);
        
        iterator = current->Node.Function.Variables;
        retValOffset = 2;
        while (iterator != NULL)
        {
            if (iterator->Node.Variable.VarKind == kLocal)
                iterator = NULL;
            else
            {
                switch (iterator->Node.Variable.Type)
                {
                case INT:
                case BOOL:
                    retValOffset += 1;
                    break;
                case STRING:
                    retValOffset += 100;
                    break;
                default:
                    break;
                }
                iterator = iterator->Node.Variable.Next;
            }
        }
        ((SymTabData*)symtab_get(funcSymTab, current->Node.Function.Name))->RetValOffset = retValOffset;

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
                formalOffset += INT_SIZE;
                offsetCalc(current->Node.Variable.Next);
                formalOffset -= INT_SIZE;
                ((SymTabData*)symtab_get(varSymTab, current->Node.Variable.Name))->Offset = formalOffset;
                return;
            case STRING:
                formalOffset += STRING_SIZE;
                offsetCalc(current->Node.Variable.Next);
                formalOffset -= STRING_SIZE;
                ((SymTabData*)symtab_get(varSymTab, current->Node.Variable.Name))->Offset = formalOffset;
                return;
            default:
                return;
            }
        case kLocal:
            switch (current->Node.Variable.Type)
            {
            case BOOL:
            case INT:
                localOffset -= INT_SIZE;
                break;
            case STRING:
                localOffset -= STRING_SIZE;
                break;
            default:
                break;
            }
            ((SymTabData*)symtab_get(varSymTab, current->Node.Variable.Name))->Offset = localOffset;
            offsetCalc(current->Node.Variable.Next);
            return;
        default:
            return;
        }
    default:
        return;
    }
}