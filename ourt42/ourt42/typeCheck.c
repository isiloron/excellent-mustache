#include <stdio.h>
#include "typeCheck.h"


eType typeCheck(t_tree current, t_tree funcVars)
{
	eType left;
	eType right;
	if (current == NULL)
		return;

	switch (current->Kind)
	{
	case kProgram:
	case kFunction:
	case kVariable:
	case kAssign:
	case kIf:
	case kWhile:
	case kRead:
	case kWrite:
	case kReturn:
	case kFuncCallStmnt:
	case kActual:
	case kUnary:
		if(typeCheck(current->Node.Binary.LeftOperand, funcVars)==INVALID_TYPE)
			typeCheck(current->Node.Binary.LeftOperand, funcVars)
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
	case kRValue:
		return lookup(funcVars, current->Node.RValue.Id);
	default:
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