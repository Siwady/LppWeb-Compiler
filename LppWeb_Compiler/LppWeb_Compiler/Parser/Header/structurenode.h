#ifndef STRUCTURENODE_H
#define STRUCTURENODE_H
#include <string>
using namespace std;

class StructureNode
{
public:
	virtual ~StructureNode()
	{
	}

	StructureNode();
    int Row;
    int Column;
    virtual string ToXML(int i);
    virtual void ValidateSemantc();
	virtual void Interpret();
};

#endif // STRUCTURENODE_H
