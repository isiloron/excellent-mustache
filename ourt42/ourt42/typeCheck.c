#include <stdio.h>
#include "typeCheck.h"

eType funcReturnType;
eType returnType;
eType left;
eType right;
eType unaryExprType;

eType typeCheck(t_tree current, t_tree funcVars)
{
	if (current == NULL)
		return;

	switch (current->Kind)
	{
	case kProgram:
		return current->Node.Program.Functions;
	case kFunction:
		funcReturnType = current->Node.Function.Type;
		typeCheck(current->Node.Function.Variables, funcVars);
		typeCheck(current->Node.Function.Stmnts, funcVars);
		return current->Node.Function.Next;
	case kVariable:
		typeCheck(current->Node.Variable.Type, funcVars);
		return typeCheck(current->Node.Variable.Next, funcVars);
	case kAssign:
		if (lookup(funcVars, current->Node.Assign.Id) != typeCheck(current->Node.Assign.Expr, funcVars))
		{
			fprintf(stderr, "Assignop: id and expr not of same type! Line: %d ", current->LineNr);
			return INVALID_TYPE;
		}
		else
			return typeCheck(current->Node.Assign.Next, funcVars);
	case kIf:
		typeCheck(current->Node.If.Expr, funcVars);
		typeCheck(current->Node.If.Then, funcVars);
		typeCheck(current->Node.If.Else, funcVars);
		return typeCheck(current->Node.If.Next, funcVars);
	case kWhile:
		typeCheck(current->Node.While.Expr, funcVars);
		typeCheck(current->Node.While.Stmnt, funcVars);
		return typeCheck(current->Node.While.Next, funcVars);
	case kRead:
		lookup(funcVars, current->Node.Read.Id);
		return typeCheck(current->Node.Read.Next, funcVars);
	case kWrite:
		typeCheck(current->Node.Write.Expr, funcVars);
		return typeCheck(current->Node.Write.Next, funcVars);
	case kReturn:
		returnType = typeCheck(current->Node.Return.Expr, funcVars);
		if (returnType != VOID)
		{
			if (returnType != funcReturnType)
			{
				fprintf(stderr, "Returnvalue: incorrect type! Line: %d ", current->LineNr);
				return INVALID_TYPE;
			}
			else
				typeCheck(current->Node.Return.Next, funcVars);
			return returnType;
		}
		else
			return VOID;
	case kFuncCallStmnt:
		typeCheck(current->Node.FuncCallStmnt.Actuals, funcVars);
		return typeCheck(current->Node.FuncCallStmnt.Next, funcVars);
	case kActual:
		typeCheck(current->Node.Actual.Expr, funcVars);
		return typeCheck(current->Node.Actual.Next, funcVars);
	case kUnary:
		unaryExprType = typeCheck(current->Node.Unary.Expr, funcVars);
		if (unaryExprType == INVALID_TYPE
			|| unaryExprType == STRING
			|| unaryExprType == VOID
			|| unaryExprType == BOOL && current->Node.Unary.Operator == NEG)
		{
			fprintf(stderr, "Unary expr, invalid type! Line: %d ", current->LineNr);
			return INVALID_TYPE;
		}
		else
			return unaryExprType;
	case kBinary:
		left = typeCheck(current->Node.Binary.LeftOperand, funcVars);
		right = typeCheck(current->Node.Binary.RightOperand, funcVars);

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
		return typeCheck(current->Node.FuncCallExpr.Actuals, funcVars);
	case kRValue:
		return lookup(funcVars, current->Node.RValue.Id);
	default:
		fprintf(stderr, "What the hell is this shit? 0o Line: %d ", current->LineNr);
		return INVALID_TYPE;
		break;
	}
}

eType lookup(t_tree funcVars, char* id)
{
	if (funcVars == NULL)
		return INVALID_TYPE;

	if (strcmp(funcVars->Node.Variable.Name, id) == 0)
		return funcVars->Node.Variable.Type;
	else
		return lookup(funcVars->Node.Variable.Next, id);
}