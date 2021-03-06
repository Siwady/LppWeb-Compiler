#ifndef DECLAREVARIABLENODE_H
#define DECLAREVARIABLENODE_H
#include <string>
#include <list>
#include "Parser/Header/typenode.h"
#include "Parser/Header/programheadernode.h"
#include "helper.h"
using namespace std;

class DeclareVariableNode:public ProgramHeaderNode
{
public:
    TypeNode* Type;
    list<string>* IDs;
    DeclareVariableNode(TypeNode* type,list<string>* ids,int row,int column);

    string ToXML(int i);

    // ProgramHeaderNode interface
public:
    void ValidateSemantic();
};

#endif // DECLAREVARIABLENODE_H
