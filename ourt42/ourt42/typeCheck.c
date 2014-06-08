#include <stdio.h>
#include "typeCheck.h"
#include "ast.h"

eType funcReturnType;
eType returnType;
eType left;
eType right;
eType unaryExprType;
t_symtab *funcSymTab, *varSymTab;
char *currentFuncName;

eType typeCheck(t_tree current)
{
	if (current == NULL)
		return;

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
		if (typeCheck(current->Node.If.Expr) == VALID_TYPE
			&& typeCheck(current->Node.If.Then) == VALID_TYPE
			&& typeCheck(current->Node.If.Else) == VALID_TYPE
			&& typeCheck(current->Node.If.Next) == VALID_TYPE)
			return VALID_TYPE;
		else
			return INVALID_TYPE;
	case kWhile:
		if (typeCheck(current->Node.While.Expr) == VALID_TYPE
			&& typeCheck(current->Node.While.Stmnt) == VALID_TYPE
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
			return INVALID_TYPE;
	case kFuncCallStmnt:
		if (typeCheck(current->Node.FuncCallStmnt.Actuals) == VALID_TYPE)
			return typeCheck(current->Node.FuncCallStmnt.Next);
		else
			return INVALID_TYPE;
	case kActual:
		typeCheck(current->Node.Actual.Expr);
		return typeCheck(current->Node.Actual.Next);
	case kUnary:
		unaryExprType = typeCheck(current->Node.Unary.Expr);
		if (current->Node.Unary.Operator == NOT
			&& (unaryExprType == BOOL || unaryExprType == INT))
			return BOOL;
		else
		{
			fprintf(stderr, "Unary expr, invalid type! Line: %d ", current->LineNr);
			return INVALID_TYPE;
		}
	case kBinary:
		left = typeCheck(current->Node.Binary.LeftOperand);
		right = typeCheck(current->Node.Binary.RightOperand);

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
			else
				return left;
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
		return typeCheck(current->Node.FuncCallExpr.Actuals);
	case kRValue:
		return lookup(current->Node.RValue.Id);
	default:
		fprintf(stderr, "What the hell is this shit? 0o Line: %d ", current->LineNr);
		return INVALID_TYPE;
		break;
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