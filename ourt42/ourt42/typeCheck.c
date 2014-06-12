#include <stdio.h>
#include "typeCheck.h"
#include "ast.h"


t_symtab *funcSymTab, *varSymTab;
char *currentFuncName;

eType typeCheck(t_tree current)
{
    eType left;
    eType right;
    eType exprType;
    UNOP_KIND unaryOp;
    BINOP_KIND binaryOp;

	if (current == NULL)
		return VALID_TYPE;

	switch (current->Kind)
	{
	case kProgram:
		funcSymTab = current->Node.Program.SymTab;
		return typeCheck(current->Node.Program.Functions);
	case kFunction:
		varSymTab = current->Node.Function.SymTab;
		currentFuncName = current->Node.Function.Name;
		if (typeCheck(current->Node.Function.Stmnts) == VALID_TYPE
			&& typeCheck(current->Node.Function.Next) == VALID_TYPE)
			return VALID_TYPE;
		else
			return INVALID_TYPE;
	case kAssign:
		if (lookup(varSymTab, current->Node.Assign.Id) == typeCheck(current->Node.Assign.Expr))
			return typeCheck(current->Node.Assign.Next);
		else
		{
			fprintf(stderr, "Assignop: id and expr not of same type! Line: %d ", current->LineNr);
			return INVALID_TYPE;
		}
	case kIf:
        exprType = typeCheck(current->Node.If.Expr);
        if (exprType != BOOL)
        {
            fprintf(stderr,"Unexpected type in condition! Line: %d\n",current->LineNr);
            return INVALID_TYPE;
        }
        else if(typeCheck(current->Node.If.Then) == VALID_TYPE
			&& typeCheck(current->Node.If.Else) == VALID_TYPE
			&& typeCheck(current->Node.If.Next) == VALID_TYPE)
			return VALID_TYPE;
		else
			return INVALID_TYPE;
	case kWhile:
        exprType = typeCheck(current->Node.While.Expr);
        if (exprType != BOOL)
        {
            fprintf(stderr, "Unexpected type in condition! Line: %d\n", current->LineNr);
            return INVALID_TYPE;
        }
		else if (typeCheck(current->Node.While.Stmnt) == VALID_TYPE
			&& typeCheck(current->Node.While.Next) == VALID_TYPE)
			return VALID_TYPE;
        else
            return INVALID_TYPE;
    case kRead:
        if (((SymTabData*)symtab_get(varSymTab,current->Node.Read.Id))->Type != INVALID_TYPE)
            return typeCheck(current->Node.Read.Next);
        else
            return INVALID_TYPE;
    case kWrite:
        if (typeCheck(current->Node.Write.Expr) != INVALID_TYPE)
            return typeCheck(current->Node.Write.Next);
        else
            return INVALID_TYPE;
	case kReturn:
		if (lookup(funcSymTab, currentFuncName) == VOID)
		{
			fprintf(stderr, "Void function cannot return.\n");
			return INVALID_TYPE;
		}
		else if (typeCheck(current->Node.Return.Expr) == lookup(funcSymTab, currentFuncName)
			&& typeCheck(current->Node.Return.Next) == VALID_TYPE)
			return VALID_TYPE;
        else
        {
            fprintf(stderr,"Value type doen't match function return type! Line: %d",current->LineNr);
            return INVALID_TYPE;
        }
	case kFuncCallStmnt:
        if (checkActuals(current->Node.FuncCallStmnt.Actuals,
            ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallStmnt.FuncName))->Vars) == VALID_TYPE)
            return typeCheck(current->Node.FuncCallStmnt.Next);
        else
        {
            fprintf(stderr, "Actuals doesn't match parameters! Line: %d\n", current->LineNr);
            return INVALID_TYPE;
        }
	case kUnary:
		exprType = typeCheck(current->Node.Unary.Expr);
        unaryOp = current->Node.Unary.Operator;

        if (unaryOp == NOT
            && exprType == BOOL)
            return BOOL;
        else if (unaryOp == NEG
            && exprType == INT)
            return INT;
		else
		{
			fprintf(stderr, "Unary expr, invalid type! Line: %d ", current->LineNr);
			return INVALID_TYPE;
		}
	case kBinary:
		left = typeCheck(current->Node.Binary.LeftOperand);
		right = typeCheck(current->Node.Binary.RightOperand);
        binaryOp = current->Node.Binary.Operator;

        switch (binaryOp)
        {
        case PLUS:
        case MINUS:
        case MULT:
        case DIV:
            if (left == INT && right == INT)
                return INT;
            else
            {
                fprintf(stderr, "Binary operator type error! Requires INT operands! Line: %d\n", current->LineNr);
                return INVALID_TYPE;
            }
        case OR:
        case AND:
            if (left == BOOL && right == BOOL)
                return BOOL;
            else
            {
                fprintf(stderr, "Binary operator type error! Requires BOOL operands! Line: %d\n", current->LineNr);
                return INVALID_TYPE;
            }
        case EQ:
            if (left != INVALID_TYPE && right != INVALID_TYPE && left == right)
                return BOOL;
            else
            {
                fprintf(stderr, "Binary operator type error! Line: %d\n", current->LineNr);
                return INVALID_TYPE;
            }
        case LT:
        case LE:
            if (left == right && left != BOOL && right != BOOL)
                return BOOL;
            else
            {
                fprintf(stderr, "Binary operator type error! Line: %d\n", current->LineNr);
                return INVALID_TYPE;
            }
        default:
            fprintf(stderr, "Unexpected binary operator! Line: %d\n",current->LineNr);
            return INVALID_TYPE;
        }
	case kIntConst:
		return INT;
	case kBoolConst:
		return BOOL;
	case kStringConst:
		return STRING;
	case kFuncCallExpr:
        if (checkActuals(current->Node.FuncCallExpr.Actuals,
            ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallExpr.FuncName))->Vars) == VALID_TYPE)
            return ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallExpr.FuncName))->Type;
        else
        {
            fprintf(stderr, "Actuals doesn't match parameters! Line: %d", current->LineNr);
            return INVALID_TYPE;
        }
	case kRValue:
		return lookup(varSymTab, current->Node.RValue.Id);
	default:
		return VALID_TYPE;
	}
}

eType lookup(t_symtab *SymTab, char* id)
{
	SymTabData *data;
	data = (SymTabData *)symtab_get(SymTab, id);
	if (data == NULL)
		return INVALID_TYPE;
	return data->Type;
}

eType checkActuals(t_tree actual, t_tree parameter)
{
    if (actual == NULL)
    {
        if (parameter == NULL
            || parameter->Node.Variable.VarKind == kLocal)
            return VALID_TYPE;
        else
            return INVALID_TYPE;
    }
    else //actual != null
    {
        if (parameter == NULL
            || parameter->Node.Variable.VarKind == kLocal
            || typeCheck(actual->Node.Actual.Expr) != parameter->Node.Variable.Type)
            return INVALID_TYPE;
        else
            return checkActuals(actual->Node.Actual.Next, parameter->Node.Variable.Next);
    }
}