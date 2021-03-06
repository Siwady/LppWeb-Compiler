#include "functionnode.h"
#include "../../helper.h"
#include "../../Semantic/symboltable.h"
#include "../../Semantic/Type/functiontype.h"


FunctionNode::FunctionNode(string id, list<ParameterNode *> *param, TypeNode *type, list<DeclareVariableNode *> *vars, list<StatementNode *> *statements, int row, int column)
{
    this->ID=id;
    this->Params=param;
    this->ReturnType=type;
    this->Variables=vars;
    this->Statements=statements;
    this->Row=row;
    this->Column=column;
	
}

FunctionNode::FunctionNode()
{
	
}


string FunctionNode::ToXML(int i)
{
    string re=Helper::GetIdentation(i)+"<Function>\n";
    re+=Helper::GetIdentation(i+1)+"<ID>\n";
    re+=Helper::GetIdentation(i+2)+ID+"\n";
    re+=Helper::GetIdentation(i+1)+"</ID>\n";
    re+=Helper::GetIdentation(i+1)+"<ParamList>\n";
    for(int j=0;j<Params->size();j++){
        re+=Helper::GetElementParameterNode(Params,j)->ToXML(i+2);
    }
    re+=Helper::GetIdentation(i+1)+"</ParamList>\n";
    re+=ReturnType->ToXML(i+1);
    re+=Helper::GetIdentation(i+1)+"<DeclareVariableList>\n";
    for(int j=0;j<Variables->size();j++){
        re+=Helper::GetElementDeclareVariableNode(Variables,j)->ToXML(i+2);
    }
    re+=Helper::GetIdentation(i+1)+"</DeclareVariableList>\n";

    re+=Helper::GetIdentation(i+1)+"<StatementList>\n";
    for(int j=0;j<Statements->size();j++){
        re+=Helper::GetElementStatementNode(Statements,j)->ToXML(i+2);
    }
    re+=Helper::GetIdentation(i+1)+"</StatementList>\n";

    re+=Helper::GetIdentation(i)+"</Function>\n";
    return re;
}

void FunctionNode::Interpret()
{
}

FunctionNode::~FunctionNode()
{
}

void FunctionNode::ValidateSemantic()
{
	list<ParameterNode*>::const_iterator iterator;
	ParameterNode* temp;
	for (int i=0; i<Params->size(); i++) {
		
		Helper::GetElementParameterNode(Params,i)->ValidateSemantic();
	}

	SymbolTable::GetInstance()->ExistType(ReturnType->OfType);

	FunctionType * t = new FunctionType(Helper::GetTypeFromTypeNode(ReturnType), Params,Variables,Statements);
	t->DefaultValue();
	SymbolTable::GetInstance()->DeclareVariable(ID, t);
	
	SymbolTable::GetInstance()->DeclareFunctionVariable(ID, t);

	list<StatementNode*>::const_iterator iterator2;
	StatementNode* temp2;
	SymbolTable::SetInFunction(ID);
	SymbolTable::GetInstance()->ReturnType = Helper::GetTypeFromTypeNode(ReturnType);
	for (iterator2 = Statements->begin(); iterator2 != Statements->end(); ++iterator2) {
		temp2 = *iterator2;
		temp2->ValidateSemantic();
	}
	SymbolTable::SetInFunction("");
	SymbolTable::GetInstance()->ReturnType = nullptr;
}