
#include <stdio.h>
#include "nameAnalysis.h"

int nameAnalysis(t_tree current, t_symtab *funcSymTab, t_symtab *varSymTab)
{
    if (current == NULL)
        return 1;

    switch (current->Kind)
    {
    case kProgram:
        funcSymTab = symtab_create();
        if (nameAnalysis(current->Node.Program.Functions, funcSymTab, varSymTab))
        {
            symtab_destroy(funcSymTab, NULL);
            return 1;
        }
        else
        {
            symtab_destroy(funcSymTab, NULL);
            return 0;
        }
    case kFunction:
        varSymTab = symtab_create();
        if (symtab_exist(funcSymTab, current->Node.Function.Name))
        {
            fprintf(stderr, "Function name %s allready defined! Line: %d ", current->Node.Function.Name, current->LineNr);
            symtab_destroy(varSymTab, NULL);
            return 0;
        }
        symtab_add(funcSymTab, current->Node.Function.Name);
        if (nameAnalysis(current->Node.Function.Variables, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.Function.Stmnts, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.Function.Next, funcSymTab, varSymTab))
        {
            symtab_destroy(varSymTab, NULL);
            return 1;
        }
        else
        {
            symtab_destroy(varSymTab, NULL);
            return 0;
        }
    case kVariable:
        if (symtab_exist(varSymTab, current->Node.Variable.Name))
        {
            fprintf(stderr, "Variable %s allready defined! Line: %d ", current->Node.Variable.Name, current->LineNr);
            return 0;
        }
        else
        {
            symtab_add(varSymTab, current->Node.Variable.Name);
            return nameAnalysis(current->Node.Variable.Next, funcSymTab, varSymTab);
        }
    case kAssign:
        if (symtab_exist(varSymTab, current->Node.Assign.Id))
        {
            return nameAnalysis(current->Node.Assign.Expr, funcSymTab, varSymTab)
                && nameAnalysis(current->Node.Assign.Next, funcSymTab, varSymTab);
        }
        else
        {
            fprintf(stderr, "Variable %s is not defined! Line: %d ", current->Node.Assign.Id, current->LineNr);
            return 0;
        }
    case kIf:
        return nameAnalysis(current->Node.If.Expr, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.If.Then, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.If.Else, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.If.Next, funcSymTab, varSymTab);
    case kWhile:
        return nameAnalysis(current->Node.While.Expr, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.While.Stmnt, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.While.Next, funcSymTab, varSymTab);
    case kRead:
        if (symtab_exist(varSymTab, current->Node.Read.Id))
        {
            return nameAnalysis(current->Node.Read.Next, funcSymTab, varSymTab);
        }
        else
        {
            fprintf(stderr, "Variable %s is not defined! Line: %d ", current->Node.Assign.Id, current->LineNr);
            return 0;
        }
    case kWrite:
        return nameAnalysis(current->Node.Write.Expr, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.Write.Next, funcSymTab, varSymTab);
    case kReturn:
        return nameAnalysis(current->Node.Return.Expr, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.Return.Next, funcSymTab, varSymTab);
    case kFuncCallStmnt:
        if (symtab_exist(funcSymTab, current->Node.FuncCallStmnt.FuncName))
        {
            return nameAnalysis(current->Node.FuncCallStmnt.Actuals, funcSymTab, varSymTab)
                && nameAnalysis(current->Node.FuncCallStmnt.Next, funcSymTab, varSymTab);
        }
        else
        {
            fprintf(stderr, "Function name %s is not defined! Line: %d ", current->Node.FuncCallStmnt.FuncName, current->LineNr);
            return 0;
        }
    case kActual:
        return nameAnalysis(current->Node.Actual.Expr, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.Actual.Next, funcSymTab, varSymTab);
    case kUnary:
        return nameAnalysis(current->Node.Unary.Expr, funcSymTab, varSymTab);
    case kBinary:
        return nameAnalysis(current->Node.Binary.LeftOperand, funcSymTab, varSymTab)
            && nameAnalysis(current->Node.Binary.RightOperand, funcSymTab, varSymTab);
    case kFuncCallExpr:
        if (symtab_exist(funcSymTab, current->Node.FuncCallExpr.FuncName))
        {
            return nameAnalysis(current->Node.FuncCallExpr.Actuals, funcSymTab, varSymTab);
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