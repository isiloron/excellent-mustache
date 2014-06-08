
#include "codeGen.h"

tCode* codeBuffer;
t_symtab *funcSymTab, *varSymTab;
t_tree currentFunction;
t_tree iterator;

void codeGen(char* lstname)
{
	generateCode(treeRoot);
	code_buffer_print(codeBuffer, lstname);
	code_buffer_destroy(codeBuffer);
	return;
}

void generateCode(t_tree current)
{
    int offset;
    eType type;
    tInstr *instrPtr1, *instrPtr2;
    int instrAddr;

	if (current == NULL)
		return;

	switch (current->Kind)
	{
	case kProgram:
		funcSymTab = current->Node.Program.SymTab;
		codeBuffer = code_buffer_create();
		generateCode(current->Node.Program.Functions);
		
		return;
	case kFunction:
		varSymTab = current->Node.Function.SymTab;
        currentFunction = current;
		((SymTabData*)symtab_get(funcSymTab, current->Node.Function.Name))->funcStart = get_next_address(codeBuffer);
        emit_fun_label(codeBuffer, current->Node.Function.Name);
		emit_link(codeBuffer);
		generateCode(current->Node.Function.Variables);
		generateCode(current->Node.Function.Stmnts);
		emit_unlink(codeBuffer);
		emit_rts(codeBuffer);
		generateCode(current->Node.Function.Next);
		return;
	case kVariable:
		if (current->Node.Variable.VarKind == kLocal)
		{
			if (current->Node.Variable.Type == STRING)
				emit_decl(codeBuffer, 100, NULL);
			else
				emit_decl(codeBuffer, 1, NULL);
		}
		generateCode(current->Node.Variable.Next);
		return;
	case kAssign:
		offset = ((SymTabData*)symtab_get(varSymTab, current->Node.Assign.Id))->Offset;
		type = ((SymTabData*)symtab_get(varSymTab, current->Node.Assign.Id))->Type;
		emit_lval(codeBuffer, offset, NULL);
		generateCode(current->Node.Assign.Expr);
		emit_ass(codeBuffer, type);
		generateCode(current->Node.Assign.Next);
		return;
	case kIf:
		generateCode(current->Node.If.Expr);
		instrPtr1 = emit_brf(codeBuffer);
		generateCode(current->Node.If.Then);
		instrPtr2 = emit_bra(codeBuffer);
		set_address(instrPtr1, get_next_address(codeBuffer));
		generateCode(current->Node.If.Else);
		set_address(instrPtr2, get_next_address(codeBuffer));
		generateCode(current->Node.If.Next);
		return;
	case kWhile:
		instrAddr = get_next_address(codeBuffer);
		generateCode(current->Node.While.Expr);
		instrPtr1 = emit_brf(codeBuffer);
		generateCode(current->Node.While.Stmnt);
		set_address(emit_bra(codeBuffer), instrAddr);
		set_address(instrPtr1, get_next_address(codeBuffer));
		generateCode(current->Node.While.Next);
		return;
	case kRead:
		offset = ((SymTabData*)symtab_get(varSymTab, current->Node.Read.Id))->Offset;
		type = ((SymTabData*)symtab_get(varSymTab, current->Node.Read.Id))->Type;
		emit_lval(codeBuffer, offset, NULL);
		emit_read(codeBuffer, type);
        emit_ass(codeBuffer, type);
		generateCode(current->Node.Read.Next);
		return;
	case kWrite:
		type = typeCheck(current->Node.Write.Expr);
		generateCode(current->Node.Write.Expr);
		emit_write(codeBuffer, type);
		emit_pop(codeBuffer, type==STRING?100:1);
		generateCode(current->Node.Write.Next);
		return;
	case kReturn:
        offset = ((SymTabData*)symtab_get(funcSymTab, currentFunction->Node.Function.Name))->RetValOffset;
		type = typeCheck(current->Node.Return.Expr);
		emit_lval(codeBuffer, offset, NULL);
		generateCode(current->Node.Return.Expr);
		emit_ass(codeBuffer, type);
		emit_unlink(codeBuffer);
		emit_rts(codeBuffer);
		generateCode(current->Node.Return.Next);
		return;
	case kFuncCallStmnt:
		type = ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallStmnt.FuncName))->Type;
		instrAddr = ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallStmnt.FuncName))->funcStart;
		if (type != VOID)
			emit_decl(codeBuffer,type == STRING ? 100 : 1, NULL);
		generateCode(current->Node.FuncCallStmnt.Actuals);
		set_address(emit_bsr(codeBuffer), instrAddr);
		popActuals(current->Node.FuncCallStmnt.Actuals);
		type = ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallStmnt.FuncName))->Type;
		if (type != VOID)
			emit_pop(codeBuffer, type == STRING ? 100 : 1);
		generateCode(current->Node.FuncCallStmnt.Next);
		return;
	case kActual:
		generateCode(current->Node.Actual.Next);
		generateCode(current->Node.Actual.Expr);
		return;
	case kUnary:
		generateCode(current->Node.Unary.Expr);
		if (current->Node.Unary.Operator == NOT)
			emit_not(codeBuffer);
		else
			emit_neg(codeBuffer);
		return;
	case kBinary:
		generateCode(current->Node.Binary.LeftOperand);
		generateCode(current->Node.Binary.RightOperand);
		switch (current->Node.Binary.Operator)
		{
		case PLUS:
			emit_add(codeBuffer);
			return;
		case MINUS:
			emit_sub(codeBuffer);
			return;
		case MULT:
			emit_mult(codeBuffer);
			return;
		case DIV:
			emit_div(codeBuffer);
			return;
		case EQ:
			type = typeCheck(current->Node.Binary.LeftOperand);
			emit_eq(codeBuffer, type);
			return;
		case LE:
			type = typeCheck(current->Node.Binary.LeftOperand);
			emit_le(codeBuffer, type);
			return;
		case LT:
			type = typeCheck(current->Node.Binary.LeftOperand);
			emit_lt(codeBuffer, type);
			return;
		case OR:
			emit_or(codeBuffer);
			return;
		case AND:
			emit_and(codeBuffer);
			return;
		default:
			return;
		}
	case kIntConst:
		emit_push_int(codeBuffer, current->Node.IntConst.Value);
		return;
	case kBoolConst:
		emit_push_bool(codeBuffer, current->Node.BoolConst.Value);
		return;
	case kStringConst:
		emit_push_string(codeBuffer, current->Node.StringConst.Value);
		return;
	case kFuncCallExpr:
		type = ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallExpr.FuncName))->Type;
		instrAddr = ((SymTabData*)symtab_get(funcSymTab, current->Node.FuncCallExpr.FuncName))->funcStart;
		emit_decl(codeBuffer, type == STRING ? 100 : 1, NULL);
        generateCode(current->Node.FuncCallExpr.Actuals);
		set_address(emit_bsr(codeBuffer), instrAddr);
        popActuals(current->Node.FuncCallExpr.Actuals);
		return;
	case kRValue:
		type = ((SymTabData*)symtab_get(varSymTab, current->Node.RValue.Id))->Type;
		offset = ((SymTabData*)symtab_get(varSymTab, current->Node.RValue.Id))->Offset;
		emit_rval(codeBuffer, type, offset, NULL);
		return;
	default:
		return;
	}
}

void popActuals(t_tree actual)
{
    eType type;

	if (actual == NULL)
		return;

	type = typeCheck(actual->Node.Actual.Expr);
	emit_pop(codeBuffer, type == STRING ? 100 : 1);
    popActuals(actual->Node.Actual.Next);
	return;
}