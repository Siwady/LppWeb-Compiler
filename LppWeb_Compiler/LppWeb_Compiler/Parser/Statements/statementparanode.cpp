#include "statementparanode.h"
#include "../../Semantic/semanticexception.h"
#include "../../helper.h"
#include "../../Semantic/symboltable.h"
#include "../../Interpret/Values/enterovalue.h"

StatementParaNode::~StatementParaNode()
{
	delete Variable;
	delete FirstExpression;
	delete SecondExpression;
	delete Statements;
}

void StatementParaNode::Interpret()
{
	EnteroValue* i= dynamic_cast<EnteroValue*>(FirstExpression->Interpret());
	EnteroValue* end = dynamic_cast<EnteroValue*>(SecondExpression->Interpret());
	
	SymbolTable::GetInstance()->SetVariableValue(Variable->ID, FirstExpression->Interpret());
	for (i->value; i->value <= end->value; i->value++)
	{
		SymbolTable::GetInstance()->SetVariableValue(Variable->ID, i);
		for (int j = 0; j < Statements->size(); j++)
		{
			Helper::GetElementStatementNode(Statements, j)->Interpret();
		}
	}
}

StatementParaNode::StatementParaNode(VariableNode *var, ExpressionNode *firstExp, ExpressionNode *secondExp, list<StatementNode *> *ls, int row, int column)
{
    this->Variable=var;
    this->FirstExpression=firstExp;
    this->SecondExpression=secondExp;
    this->Statements=ls;
    this->Row=row;
    this->Column=column;
}



string StatementParaNode::ToXML(int i)
{
    string re=Helper::GetIdentation(i)+"<StatementPara>\n";

    re+=Variable->ToXML(i+1);
    re+=FirstExpression->ToXML(i+1);
    re+=SecondExpression->ToXML(i+1);

    re+=Helper::GetIdentation(i+1)+"<StatementList>\n";
    for(int j=0;j<Statements->size();j++){
        re+=Helper::GetElementStatementNode(Statements,j)->ToXML(i+2);
    }
    re+=Helper::GetIdentation(i+1)+"</StatementList>\n";

    re+=Helper::GetIdentation(i)+"</StatementPara>\n";
    return re;
}


void StatementParaNode::ValidateSemantic()
{
    if(Variable->ValidateSemantic()->Name!="Entero")
    {
        throw SemanticException("Se esperaba Entero ,Fila:"+to_string(Variable->Row)+",Columna:"+to_string(Variable->Column));
    }
    if(FirstExpression->ValidateSemantic()->Name!="Entero")
    {
        throw SemanticException("Se esperaba Entero ,Fila:"+to_string(FirstExpression->Row)+",Columna:"+to_string(FirstExpression->Column));
    }
    if(SecondExpression->ValidateSemantic()->Name!="Entero")
    {
        throw SemanticException("Se esperaba Entero ,Fila:"+to_string(SecondExpression->Row)+",Columna:"+to_string(SecondExpression->Column));
    }

    list<StatementNode*>::const_iterator iterator;
    StatementNode* temp;
    for (iterator = Statements->begin(); iterator != Statements->end(); ++iterator) {
        temp=*iterator;
        temp->ValidateSemantic();
    }
}
