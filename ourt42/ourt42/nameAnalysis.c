
#include <stdio.h>
#include "ast.h"

t_symtab *funcSymTab;
t_symtab *varSymTab;

int nameAnalysis(t_tree current)
{
    if (current == NULL)
        return 1;

    switch (current->Kind)
    {
    case kProgram:
        current->Node.Program.SymTab = symtab_create();
        funcSymTab = current->Node.Program.SymTab;
        return nameAnalysis(current->Node.Program.Functions);
    case kFunction:
        current->Node.Function.SymTab = symtab_create();
        varSymTab = current->Node.Function.SymTab;
        if (symtab_exist(funcSymTab, current->Node.Function.Name))
        {
            fprintf(stderr, "Function name %s allready defined! Line: %d ", current->Node.Function.Name, current->LineNr);
            return 0;
        }
        symtab_add(funcSymTab, current->Node.Function.Name);
        symtab_set(funcSymTab, current->Node.Function.Name, (void *)malloc(sizeof(SymTabData)));
        ((SymTabData*)symtab_get(funcSymTab, current->Node.Function.Name))->Type = current->Node.Function.Type;
        return nameAnalysis(current->Node.Function.Variables)
            && nameAnalysis(current->Node.Function.Stmnts)
            && nameAnalysis(current->Node.Function.Next);
    case kVariable:
        if (symtab_exist(varSymTab, current->Node.Variable.Name))
        {
            fprintf(stderr, "Variable %s allready defined! Line: %d ", current->Node.Variable.Name, current->LineNr);
            return 0;
        }
        else
        {
            symtab_add(varSymTab, current->Node.Variable.Name);
            symtab_set(varSymTab, current->Node.Variable.Name, (void *)malloc(sizeof(SymTabData)));
            ((SymTabData *)symtab_get(varSymTab, current->Node.Variable.Name))->Type = current->Node.Variable.Type;
            ((SymTabData *)symtab_get(varSymTab, current->Node.Variable.Name))->VarKind = current->Node.Variable.VarKind;
            return nameAnalysis(current->Node.Variable.Next);
        }
    case kAssign:
        if (symtab_exist(varSymTab, current->Node.Assign.Id))
        {
            return nameAnalysis(current->Node.Assign.Expr)
                && nameAnalysis(current->Node.Assign.Next);
        }
        else
        {
            fprintf(stderr, "Variable %s is not defined! Line: %d ", current->Node.Assign.Id, current->LineNr);
            return 0;
        }
    case kIf:
        return nameAnalysis(current->Node.If.Expr)
            && nameAnalysis(current->Node.If.Then)
            && nameAnalysis(current->Node.If.Else)
            && nameAnalysis(current->Node.If.Next);
    case kWhile:
        return nameAnalysis(current->Node.While.Expr)
            && nameAnalysis(current->Node.While.Stmnt)
            && nameAnalysis(current->Node.While.Next);
    case kRead:
        if (symtab_exist(varSymTab, current->Node.Read.Id))
        {
            return nameAnalysis(current->Node.Read.Next);
        }
        else
        {
            fprintf(stderr, "Variable %s is not defined! Line: %d ", current->Node.Assign.Id, current->LineNr);
            return 0;
        }
    case kWrite:
        return nameAnalysis(current->Node.Write.Expr)
            && nameAnalysis(current->Node.Write.Next);
    case kReturn:
        return nameAnalysis(current->Node.Return.Expr)
            && nameAnalysis(current->Node.Return.Next);
    case kFuncCallStmnt:
        if (symtab_exist(funcSymTab, current->Node.FuncCallStmnt.FuncName))
        {
            return nameAnalysis(current->Node.FuncCallStmnt.Actuals)
                && nameAnalysis(current->Node.FuncCallStmnt.Next);
        }
        else
        {
            fprintf(stderr, "Function name %s is not defined! Line: %d ", current->Node.FuncCallStmnt.FuncName, current->LineNr);
            return 0;
        }
    case kActual:
        return nameAnalysis(current->Node.Actual.Expr)
            && nameAnalysis(current->Node.Actual.Next);
    case kUnary:
        return nameAnalysis(current->Node.Unary.Expr);
    case kBinary:
        return nameAnalysis(current->Node.Binary.LeftOperand)
            && nameAnalysis(current->Node.Binary.RightOperand);
    case kFuncCallExpr:
        if (symtab_exist(funcSymTab, current->Node.FuncCallExpr.FuncName))
        {
            return nameAnalysis(current->Node.FuncCallExpr.Actuals);
        }
        else
        {
            fprintf(stderr, "Function name %s is not defined! Line: %d ", current->Node.FuncCallExpr.FuncName, current->LineNr);
            return 0;
        }
    case kRValue:
        if (symtab_exist(varSymTab, current->Node.RValue.Id))
        {
            return 1;
        }
        else
        {
            fprintf(stderr, "Variable %s is not defined! Line: %d ", current->Node.RValue.Id, current->LineNr);
            return 0;
        }
    default:
        return 1;
    }
}