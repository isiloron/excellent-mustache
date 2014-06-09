#include <stdio.h>
#include "ast.h"
#include "printAST.h"

void printAST(t_tree current)
{
    if (current == NULL)
		return;

    switch (current->Kind)
	{
	case kProgram:
        printAST(current->Node.Program.Functions);
		return;
	case kFunction:
        printType(current->Node.Function.Type);
        printf(current->Node.Function.Name);
        printAST(current->Node.Function.Variables);
        printAST(current->Node.Function.Stmnts);
        printAST(current->Node.Function.Next);
		return;
	case kVariable:
        if (current->Node.Variable.VarKind == kFormal)
			printf("(formal) ");
		else
			printf("(local) ");

        printType(current->Node.Variable.Type);
        printf(current->Node.Variable.Name);
        printAST(current->Node.Variable.Next);
		return;
	case kAssign:
        printf(current->Node.Assign.Id);
		printf(" = ");
        printAST(current->Node.Assign.Expr);
        printAST(current->Node.Assign.Next);
		return;
	case kIf:
		printf("if(");
        printAST(current->Node.If.Expr);
		printf(")\n{\n");
        printAST(current->Node.If.Then);
		printf("\n}");
        if (current->Node.If.Else != NULL)
		{
			printf("else\n{\n");
            printAST(current->Node.If.Else);
			printf("\n}\n");
		}
        printAST(current->Node.If.Next);
		return;
	case kWhile:
		printf("while(");
        printAST(current->Node.While.Expr);
		printf(")\n{\n");
        printAST(current->Node.While.Stmnt);
		printf("}\n");
        printAST(current->Node.While.Next);
		return;
	case kRead:
		printf("read ");
        printf(current->Node.Read.Id);
		printf(";\n");
        printAST(current->Node.Read.Next);
		return;
	case kWrite:
		printf("write ");
        printAST(current->Node.Write.Expr);
		printf(";\n");
        printAST(current->Node.Write.Next);
		return;
	case kReturn:
		printf("return ");
        printAST(current->Node.Return.Expr);
		printf(";\n");
        printAST(current->Node.Return.Next);
		return;
	case kFuncCallStmnt:
		printf(current->Node.FuncCallStmnt.FuncName);
		printf("(");
		printAST(current->Node.FuncCallStmnt.Actuals);
		printf(");\n");
		printAST(current->Node.FuncCallStmnt.Next);
		return;
	case kActual:
		printAST(current->Node.Actual.Expr);
		if (current->Node.Actual.Next != NULL)
			printf(", ");
		printAST(current->Node.Actual.Next);
		return;
	case kUnary:
		if (current->Node.Unary.Operator == NOT)
			printf("!");
		else
			printf("-");
		printAST(current->Node.Unary.Expr);
		return;
	case kBinary:
		printAST(current->Node.Binary.LeftOperand);
		printOperator(current->Node.Binary.Operator);
		printAST(current->Node.Binary.RightOperand);
		return;
	case kIntConst:
		printf("%d", current->Node.IntConst.Value);
		return;
	case kBoolConst:
		current->Node.BoolConst.Value == 1 ?
			printf("true") :
			printf("false");
		return;
	case kStringConst:
		printf(current->Node.StringConst.Value);
		return;
	case kFuncCallExpr:
		printf(current->Node.FuncCallExpr.FuncName);
		printf("(");
		printAST(current->Node.FuncCallExpr.Actuals);
		printf(")\n");
		return;
	case kRValue:
		printf(current->Node.RValue.Id);
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