#include <stdio.h>
#include "ast.h"
#include "printAST.h"

void printAST(t_tree currentNode)
{
	if (currentNode == NULL)
		return;

	switch (currentNode->Kind)
	{
	case kProgram:
		printAST(currentNode->Node.Program.Functions);
		return;
	case kFunction:
		printType(currentNode->Node.Function.Type);
		printf(currentNode->Node.Function.Name);
		printAST(currentNode->Node.Function.Variables);
		printAST(currentNode->Node.Function.Stmnts);
		printAST(currentNode->Node.Function.Next);
		return;
	case kVariable:
		if (currentNode->Node.Variable.VarKind == kFormal)
			printf("(formal) ");
		else
			printf("(local) ");

		printType(currentNode->Node.Variable.Type);
		printf(currentNode->Node.Variable.Name);
		printAST(currentNode->Node.Variable.Next);
		return;
	case kAssign:
		printf(currentNode->Node.Assign.Id);
		printf(" = ");
		printAST(currentNode->Node.Assign.Expr);
		printAST(currentNode->Node.Assign.Next);
		return;
	case kIf:
		printf("if(");
		printAST(currentNode->Node.If.Expr);
		printf(")\n{\n");
		printAST(currentNode->Node.If.Then);
		printf("\n}");
		if (currentNode->Node.If.Else != NULL)
		{
			printf("else\n{\n");
			printAST(currentNode->Node.If.Else);
			printf("\n}\n");
		}
		printAST(currentNode->Node.If.Next);
		return;
	case kWhile:
		printf("while(");
		printAST(currentNode->Node.While.Expr);
		printf(")\n{\n");
		printAST(currentNode->Node.While.Stmnt);
		printf("}\n");
		printAST(currentNode->Node.While.Next);
		return;
	case kRead:
		printf("read ");
		printf(currentNode->Node.Read.Id);
		printf(";\n");
		printAST(currentNode->Node.Read.Next);
		return;
	case kWrite:
		printf("write ");
		printAST(currentNode->Node.Write.Expr);
		printf(";\n");
		printAST(currentNode->Node.Write.Next);
		return;
	case kReturn:
		printf("return ");
		printAST(currentNode->Node.Return.Expr);
		printf(";\n");
		printAST(currentNode->Node.Return.Next);
		return;
	case kFuncCallStmnt:
		printf(currentNode->Node.FuncCallStmnt.FuncName);
		printf("(");
		printAST(currentNode->Node.FuncCallStmnt.Actuals);
		printf(");\n");
		printAST(currentNode->Node.FuncCallStmnt.Next);
		return;
	case kActual:
		printAST(currentNode->Node.Actual.Expr);
		if (currentNode->Node.Actual.Next != NULL)
			printf(", ");
		printAST(currentNode->Node.Actual.Next);
		return;
	case kUnary:
		if (currentNode->Node.Unary.Operator == NOT)
			printf("!");
		else
			printf("-");
		printAST(currentNode->Node.Unary.Expr);
		return;
	case kBinary:
		printAST(currentNode->Node.Binary.LeftOperand);
		printOperator(currentNode->Node.Binary.Operator);
		printAST(currentNode->Node.Binary.RightOperand);
		return;
	case kIntConst:
		printf("%d", currentNode->Node.IntConst.Value);
		return;
	case kBoolConst:
		currentNode->Node.BoolConst.Value == 1 ?
			printf("true") :
			printf("false");
		return;
	case kStringConst:
		printf(currentNode->Node.StringConst.Value);
		return;
	case kFuncCallExpr:
		printf(currentNode->Node.FuncCallExpr.FuncName);
		printf("(");
		printAST(currentNode->Node.FuncCallExpr.Actuals);
		printf(")\n");
		return;
	case kRValue:
		printf(currentNode->Node.RValue.Id);
		return;
	default:
		printf("Unknown node detected!\n");
	}
}

void printType(eType type)
{
	switch (type)
	{
	case VOID:
		printf("void ");
		return;
	case BOOL:
		printf("bool ");
		return;
	case INT:
		printf("int ");
		return;
	case STRING:
		printf("string ");
		return;
	default:
		printf("Some shitty type that should not be!");
		return;
	}
}

void printOperator(BINOP_KIND op)
{
	switch (op)
	{
	case PLUS:
		printf(" + ");
		return;
	case MINUS:
		printf(" - ");
		return;
	case MULT:
		printf(" * ");
		return;
	case DIV:
		printf(" / ");
		return;
	case OR:
		printf(" || ");
		return;
	case AND:
		printf(" && ");
		return;
	case EQ:
		printf(" == ");
		return;
	case LT:
		printf(" < ");
		return;
	case LE:
		printf(" <= ");
		return;
	default:
		printf("Some shitty operator that should not be!");
		return;
	}
}