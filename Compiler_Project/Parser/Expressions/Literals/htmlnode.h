#ifndef HTMLNODE_H
#define HTMLNODE_H
#include "Parser/programcodenode.h"
#include "helper.h"

class HtmlNode : public ProgramCodeNode
{
public:
    string HTML;
    HtmlNode(string html,int row,int column);

    // ProgramCodeNode interface
public:
    string ToXML(int i);

    // ProgramCodeNode interface
public:
    void ValidateSemantic();
};

#endif // HTMLNODE_H
