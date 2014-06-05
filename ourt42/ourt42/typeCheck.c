#include <stdio.h>
#include "typeCheck.h"
#include "ast.h"



eType left;
eType right;
eType exprType;
UNOP_KIND unaryOp;
BINOP_KIND binaryOp;
t_symtab *funcSymTab, *varSymTab;
char *currentFuncName;

eType typeCheck(t_tree current)
{
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
        if (exprType != BOOL && exprType != INT)
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
        if (exprType != BOOL && exprType != INT)
        {
            fprintf(stderr, "Unexpected type in condition! Line: %d\n", current->LineNr);
            return INVALID_TYPE;
        }
		else if (typeCheck(current->Node.While.Stmnt) == VALID_TYPE
			&& typeCheck(current->Node.While.Next) == VALID_TYPE)
			return VALID_TYPE;
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
            && (exprType == BOOL || exprType == INT))
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

		if (left == INVALID_TYPE || right == INVALID_TYPE)
		{
			fprintf(stderr, "Invalid type! Line: %d ", current->LineNr);
			return INVALID_TYPE;
		}
		else if (left == STRING || left == VOID || right == STRING || right == VOID)
		{
			fprintf(stderr, "Binary expresion type is incorrect! Line: %d ", current->LineNr);
			return INVALID_TYPE;
		}
		else if (left != right)
		{
			if (!((left == BOOL && right == INT) && -1 < right < 2) || !((right == BOOL && left == INT) && -1 < left < 2))
			{
				fprintf(stderr, "Right and left operand, are not of the same type! Line: %d ", current->LineNr);
				return INVALID_TYPE;
			}
		}
		else
		{
			return left;
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
        if (parameter == NULL)
            return VALID_TYPE;
        else if (parameter->Node.Variable.VarKind == kLocal)
            return VALID_TYPE;
        else //paramter is formal
            return INVALID_TYPE;
    }
    else //actual != null
    {
        if (parameter == NULL)
            return INVALID_TYPE;
        else if (parameter->Node.Variable.VarKind == kLocal)
            return INVALID_TYPE;
        else if (typeCheck(actual->Node.Actual.Expr) == parameter->Node.Variable.Type)
            return checkActuals(actual->Node.Actual.Next, parameter->Node.Variable.Next);
        else
            return INVALID_TYPE;
    }
}