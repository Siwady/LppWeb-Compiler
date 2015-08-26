#include "parser.h"

Parser::Parser(Lexer *lex)
{
    this->Lex=lex;
    InitializeTypeWords();
    InitializeStatementWords();
    ConsumeToken();
}

void Parser::ConsumeToken()
{
    this->CurrentToken = this->Lex->GetToken();
}

void Parser::InitializeTypeWords()
{
    this->TypeWords.insert(make_pair("entero",entero));
    this->TypeWords.insert(make_pair("real",real));
    this->TypeWords.insert(make_pair("booleano",booleano));
    this->TypeWords.insert(make_pair("caracter",caracter));
    this->TypeWords.insert(make_pair("cadena",cadena));
    this->TypeWords.insert(make_pair("arreglo",arreglo));
    this->TypeWords.insert(make_pair("archivo",archivo));
    this->TypeWords.insert(make_pair("id",Id));
}

void Parser::InitializeStatementWords()
{
    this->StatementWords.insert(make_pair("si",si));
    this->StatementWords.insert(make_pair("mientras",mientras));
    this->StatementWords.insert(make_pair("llamar",llamar));
    this->StatementWords.insert(make_pair("caso",caso));
    this->StatementWords.insert(make_pair("abrir",abrir));
    this->StatementWords.insert(make_pair("escribir",escribir));
    this->StatementWords.insert(make_pair("leer",leer));
    this->StatementWords.insert(make_pair("para",para));
    this->StatementWords.insert(make_pair("repita",repita));
    this->StatementWords.insert(make_pair("id",Id));
    this->StatementWords.insert(make_pair("escriba",escriba));
    this->StatementWords.insert(make_pair("cerrar",cerrar));
    this->StatementWords.insert(make_pair("retorne",retorne));
}


void Parser::Parse()
{
    Program_Code();
    if (CurrentToken->Type != Eof)
    {
        throw ParserException(string("Error sintactico,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

void Parser::Program_Code()
{
    if(CurrentToken->Type== tipo ||CurrentToken->Type== registro || CurrentToken->Type== procedimiento  ||CurrentToken->Type== funcion ||CurrentToken->Type== Id ||Lex->Contains(StatementWords,Lex->ToLowerCase(CurrentToken->Lexeme))|| CurrentToken->Type==Id ||CurrentToken->Type==declarar)
    {
        Lpp_Program();
        Program_Code();
    }else if(CurrentToken->Type==Html)
    {
        ConsumeToken();
        Program_Code();
    }else
    {
        //Epsilon
    }
}

void Parser::Lpp_Program()
{
    Program_Header();
    Statement_List();
}

void Parser::Program_Header()
{
    if(CurrentToken->Type==tipo || CurrentToken->Type==registro)
    {
        Types_List();
    }else if(CurrentToken->Type==declarar)
    {
        Declare();
    }else if(CurrentToken->Type== procedimiento  ||CurrentToken->Type== funcion)
    {
        Methods_List();
    }else
    {
        //Epsilon
    }
}

list<StructureNode*>* Parser::Types_List(list<StructureNode*>* ls)
{
    if(CurrentToken->Type==tipo || CurrentToken->Type==registro)
    {
       ls->push_back(Types_Structure());
       return Types_List(ls);
    }else
    {
        return ls;//Epsilon
    }
}

StructureNode* Parser::Types_Structure()
{
    if(CurrentToken->Type==tipo)
    {
       ConsumeToken();
       if(CurrentToken->Type==Id)
       {
           string id=CurrentToken->Lexeme;
           ConsumeToken();
           if(CurrentToken->Type==es)
           {
               ConsumeToken();
               if(Lex->Contains(TypeWords,Lex->ToLowerCase(CurrentToken->Lexeme)) || CurrentToken->Type==Id ||CurrentToken->Type==secuencial)
               {
                    TypeStructureNode* type=new TypeStructureNode(id,Type());
                    return type;
               }else{
                   throw ParserException(string("se esperaba un tipo,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
               }
           }else{
               throw ParserException(string("se esperaba ES,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
           }
       }else{
           throw ParserException(string("se esperaba un ID,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
       }
    }else if(CurrentToken->Type==registro)
    {
        ConsumeToken();
        if(CurrentToken->Type==Id)
        {
            string id=CurrentToken->Lexeme;
            ConsumeToken();
            list<DeclareVariableNode*>* ls=new list<DeclareVariableNode*>();
            ls=Declare(ls);
            if(CurrentToken->Type==fin)
            {
                ConsumeToken();
                if(CurrentToken->Type==registro)
                {
                    ConsumeToken();
                    RegisterStructureNode* reg=new RegisterStructureNode(id,ls);
                    return reg;
                }else{
                    throw ParserException(string("se esperaba registro,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                }
            }else{
                throw ParserException(string("se esperaba fin,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else{
            throw ParserException(string("se esperaba un ID,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba Tipo o Registro,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

TypeNode *Parser::Type()
{
    if(CurrentToken->Type==cadena)
    {
        ConsumeToken();
        if(CurrentToken->Type==LeftBrackets)
        {
            ConsumeToken();
            if(CurrentToken->Type==Const_entero)
            {
                int size=atoi(CurrentToken->Lexeme.c_str());
                ConsumeToken();
                if(CurrentToken->Type==RightBrackets)
                {
                    ConsumeToken();
                    CadenaTypeNode* type=new CadenaTypeNode(size);
                    return type;
                }else{
                    throw ParserException(string("se esperaba ],Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                }
            }else{
                throw ParserException(string("se esperaba una constante entera,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else{
            throw ParserException(string("se esperaba [,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else if(CurrentToken->Type==arreglo)
    {
        ConsumeToken();
        if(CurrentToken->Type==LeftBrackets)
        {
            ConsumeToken();
            if(CurrentToken->Type==Const_entero)
            {
                list<int>* dim=Array_Size();
                if(CurrentToken->Type==RightBrackets)
                {
                    ConsumeToken();
                    if(CurrentToken->Type==de)
                    {
                        ConsumeToken();

                        if(Lex->Contains(TypeWords,Lex->ToLowerCase(CurrentToken->Lexeme))|| CurrentToken->Type==Id)
                        {
                            TypeNode* t=Type();
                            ArrayTypeNode* type=new ArrayTypeNode(t,dim);
                            return type;
                        }else{
                            throw ParserException(string("se esperaba un tipo,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                        }
                    }else{
                        throw ParserException(string("se esperaba {de},Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                    }
                }else{
                    throw ParserException(string("se esperaba ],Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                }
            }else{
                throw ParserException(string("se esperaba una constante entera,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else{
            throw ParserException(string("se esperaba [,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else if(CurrentToken->Type==archivo)
    {
        ConsumeToken();
        return Arch_Type();

    }else if(CurrentToken->Type==Id ||CurrentToken->Type==entero ||CurrentToken->Type==real||CurrentToken->Type==booleano|| CurrentToken->Type==caracter)
    {
        SimpeTypeNode* type=new SimpeTypeNode(Lex->ToLowerCase(CurrentToken->Lexeme));
        ConsumeToken();
        return type;
    }/*else if(CurrentToken->Type==Op_Sub)
    {
        ConsumeToken();
        Const_Negative();
    }*/else
    {
        throw ParserException(string("se esperaba un tipo,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

TypeNode* Parser::Arch_Type()
{
    if(CurrentToken->Type==de)
    {
        ConsumeToken();
        return Type();

    }else if(CurrentToken->Type==secuencial)
    {
        SimpeTypeNode* type=new SimpeTypeNode("secuencial");
        ConsumeToken();
        return type;
    }else
    {
        throw ParserException(string("se esperaba De o Secuencial,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

list<DeclareVariableNode*>* Parser::Declare(list<DeclareVariableNode*>* ls)
{
    if(CurrentToken->Type==declarar)
    {
        ConsumeToken();
        ls->push_back(Declare_Variables());
        return Declare(ls);
    }else
    {
        return ls;//Epsilon
    }
}

DeclareVariableNode* Parser::Declare_Variables()
{
    return Variables_Group();
}

DeclareVariableNode* Parser::Variables_Group()
{
    if(Lex->Contains(TypeWords,Lex->ToLowerCase(CurrentToken->Lexeme))||CurrentToken->Type==Id)
    {
        TypeNode* type=Type();
        if(CurrentToken->Type==Id)
        {
            list<string>* ids=new list<string>();
            ids->push_back(CurrentToken->Lexeme);
            ConsumeToken();
            ids=ID_List(ids);
            DeclareVariableNode* declare=new DeclareVariableNode(type,ids);
            return declare;

        }else{
            throw ParserException(string("se esperaba un Id,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba un Tipo,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

list<string>* Parser::ID_List(list<string>* ls)
{
    if(CurrentToken->Type==comma)
    {
        ConsumeToken();
        if(CurrentToken->Type==Id)
        {
            ls->push_back(CurrentToken->Lexeme);
            ConsumeToken();
            return ID_List(ls);
        }else{
            throw ParserException(string("se esperaba un Id,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else
    {
        return ls;//Epsilon
    }
}

list<int>* Parser::Array_Size()
{
    if(CurrentToken->Type==Const_entero)
    {
        list<int>* ls=new list<int>();
        ls->push_back(atoi(CurrentToken->Lexeme.c_str()));
        ConsumeToken();

        return Integer_List(ls);
    }else{
        throw ParserException(string("se esperaba una constante entera,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

list<int> *Parser::Integer_List(list<int> * ls)
{
    if(CurrentToken->Type==comma)
    {
        ConsumeToken();
        if(CurrentToken->Type==Const_entero)
        {
            ls->push_back(atoi(CurrentToken->Lexeme.c_str()));
            ConsumeToken();

            return Integer_List(ls);
        }else{
            throw ParserException(string("se esperaba una constante entera,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else
    {
        //Epsilon
    }
}

StatementNode* Parser::Methods_List()
{
    if(CurrentToken->Type==procedimiento)
    {
        ConsumeToken();
        ProcedureNode* proc=new ProcedureNode();
        if(CurrentToken->Type==Id)
        {
            proc->ID=CurrentToken->Lexeme;
            ConsumeToken();
            proc->Params=Params_List();
            proc->Variables=Declare();
            proc->Statements=Method_Body();
            return proc;
            //Methods_List();
        }else{
            throw ParserException(string("se esperaba un ID ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }

    }else if( CurrentToken->Type==funcion)
    {
        ConsumeToken();
        FunctionNode* func=new FunctionNode();
        if(CurrentToken->Type==Id)
        {
            func->ID=CurrentToken->Lexeme;
            ConsumeToken();
            func->Params=Params_List();
            if(CurrentToken->Type==colon)
            {
                ConsumeToken();
                func->ReturnType=Type();
                func->Variables=Declare();
                func->Statements=Method_Body();
                return func;
                //Methods_List();
            }else{
                throw ParserException(string("se esperaba :  ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else{
            throw ParserException(string("se esperaba un ID ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        //Epsilon
    }
}



list<StatementNode *> *Parser::Method_Body()
{
    if(CurrentToken->Type==inicio)
    {
        ConsumeToken();
        list<StatementNode*>* ls=new list<StatementNode*>();
        if(Lex->Contains(StatementWords,Lex->ToLowerCase(CurrentToken->Lexeme))|| CurrentToken->Type==Id)
        {
            ls=Statement_List(ls);
        }else
        {
            //Epsilon
        }
        if(CurrentToken->Type==fin)
        {
            ConsumeToken();
            return ls;
        }else{
            throw ParserException(string("se esperaba Fin,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba Inicio,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

/*void Parser::Function_Body()
{
    if(CurrentToken->Type==inicio)
    {
        ConsumeToken();
        Statement_List();

        if(CurrentToken->Type==retorne)
        {
            ConsumeToken();
            Expression();
            if(CurrentToken->Type==fin)
            {
                ConsumeToken();
            }else{
                throw ParserException(string("se esperaba Fin,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else{
            throw ParserException(string("se esperaba Retorne,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }

    }else{
        throw ParserException(string("se esperaba Inicio,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}
*/
list<ParameterNode *> *Parser::Params_List()
{
    list<ParameterNode*>* ls=Declare_Params();
    if(CurrentToken->Type==LeftParent)
    {
        ConsumeToken();
        if(CurrentToken->Type==RightParent)
        {
            ConsumeToken();
            return ls;
        }else{
           throw ParserException(string("se esperaba ),Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else
    {
        return ls;//Epsilon
    }
}

list<ParameterNode*>* Parser::Declare_Params()
{
    list<ParameterNode*>* params=new list<ParameterNode*>();
    params->push_back(Param());
    return Param_Group(params);
}

list<ParameterNode *> *Parser::Param_Group(list<ParameterNode*>* ls)
{
    if(CurrentToken->Type==comma)
    {
        ConsumeToken();
        ls->push_back(Param());
        return Param_Group(ls);
    }else
    {
        return ls;//Epsilon
    }
}

ParameterNode *Parser::Param()
{

    if(CurrentToken->Type==var)
    {
        ConsumeToken();
        string t=Type();
        if(CurrentToken->Type==Id)
        {
            string id=CurrentToken->Lexeme;
            ConsumeToken();
            ParameterNode* param=new ParameterNode(t,id,true);
            return param;
        }else{
            throw ParserException(string("se esperaba un ID,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else if(Lex->Contains(TypeWords,Lex->ToLowerCase(CurrentToken->Lexeme))|| CurrentToken->Type==Id)
    {
        string t=Type();
        if(CurrentToken->Type==Id)
        {
            string id=CurrentToken->Lexeme;
            ConsumeToken();
            ParameterNode* param=new ParameterNode(t,id,false);
            return param;
        }else{
            throw ParserException(string("se esperaba un ID,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba var o un Tipo de dato,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}


list<StatementNode *> *Parser::Statement_List(list<StatementNode *> * sl)
{
    if(Lex->Contains(StatementWords,Lex->ToLowerCase(CurrentToken->Lexeme))|| CurrentToken->Type==Id)
    {
        sl->push_back(Statement());
        sl=Statement_List(sl);
    }else
    {
        return sl;//Epsilon
    }
}

StatementNode *Parser::Statement()
{
    if(CurrentToken->Type==si)
    {
        return Statement_Si();
    }else if(CurrentToken->Type==mientras)
    {
        return Statement_Mientras();
    }else if(CurrentToken->Type==llamar)
    {
        return Statement_Llamar();
    }else if(CurrentToken->Type==caso)
    {
        return Statement_Case();
    }else if(CurrentToken->Type==abrir)
    {
        return Statement_Abrir_Archivo();
    }else if(CurrentToken->Type==escribir)
    {
        return Statement_Escribir_Archivo();
    }else if(CurrentToken->Type==leer)
    {
        return Statement_Leer_Archivo();
    }else if(CurrentToken->Type==para)
    {
        return Statement_Para();
    }else if(CurrentToken->Type==repita)
    {
        return Statement_Repita();
    }else if(CurrentToken->Type==Id)
    {
       return  Statement_Assignment();
    }else if(CurrentToken->Type==escriba)
    {
        return Statement_Escriba();
    }else if(CurrentToken->Type==cerrar)
    {
        return Statement_Cerrar_Archivo();
    }else if(CurrentToken->Type==retorne)
    {
        return Statement_Return();
    }else
    {
        throw ParserException(string("se esperaba una sentencia,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Return()
{
    if(CurrentToken->Type==retorne)
    {
        ConsumeToken();
        StatementReturnNode* statement=new StatementReturnNode(Expression());
        return statement;
    }else
    {
        throw ParserException(string("se esperaba Retorne,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Si()
{
    if(CurrentToken->Type==si)
    {
        ConsumeToken();
        ExpressionNode* e=Expression();

        if(CurrentToken->Type==entonces)
        {
            ConsumeToken();
            list<StatementNode*>* lsSi=new list<StatementNode*>();
            lsSi=Statement_List(lsSi);
            Statement_Sino();
            if(CurrentToken->Type==fin)
            {
                ConsumeToken();
                if(CurrentToken->Type==si)
                {
                    StatementSiNode* statement=new StatementSiNode(e,lsSi,);
                    ConsumeToken();

                }else{
                    throw ParserException(string("se esperaba si,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                }
            }else{
                throw ParserException(string("se esperaba Fin,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else{
            throw ParserException(string("se esperaba entonces,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba Si ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

void Parser::Statement_Sino()
{
    if(CurrentToken->Type==sino)
    {
        ConsumeToken();
        Statement_SinoP();
    }else
    {
        //Epsilon
    }
}

void Parser::Statement_SinoP()
{
    if(CurrentToken->Type==si)
    {
        Statement_Si();
    }else
    {
        Statement_List();
    }
}

StatementNode *Parser::Statement_Para()
{
    if(CurrentToken->Type==para)
    {
        ConsumeToken();
        VariableNode* var=Variable();
        if(CurrentToken->Type==Op_Assignment)
        {
            ConsumeToken();
            ExpressionNode* first=Expression();
            if(CurrentToken->Type==hasta)
            {
                ConsumeToken();
                ExpressionNode* second=Expression();
                if(CurrentToken->Type==haga)
                {
                    ConsumeToken();
                    list<StatementNode*>* ls=new list<StatementNode*>();
                    StatementParaNode* statement=new StatementParaNode(var,first,second,Statement_List(ls));
                    if(CurrentToken->Type==fin)
                    {
                        ConsumeToken();
                        if(CurrentToken->Type==para)
                        {
                            ConsumeToken();
                            return statement;
                        }else{
                            throw ParserException(string("se esperaba Para,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                        }
                    }else{
                        throw ParserException(string("se esperaba Fin,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                    }
                }else{
                    throw ParserException(string("se esperaba Haga,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                }
            }else{
                throw ParserException(string("se esperaba Hasta,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else{
            throw ParserException(string("se esperaba <-,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba un Para ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

VariableNode* Parser::Variable()
{
    VariableNode* e=Simple_Variable();
    return Compuest_Variable(e);
}

VariableNode* Parser::Simple_Variable()
{
    if(CurrentToken->Type==Id)
    {
        ConsumeToken();
        return Array_Variable(CurrentToken->Lexeme);
    }else
    {
        throw ParserException(string("se esperaba un ID,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

VariableNode *Parser::Compuest_Variable(VariableNode* expressionNode)
{

    if(CurrentToken->Type==dot)
    {
        ConsumeToken();
        VariableNode* e=Variable();
        RegisterVariableNode* reg=new RegisterVariableNode(expressionNode,e);
        return reg;
    }else
    {
        return expressionNode;//Epsilon
    }
}

VariableNode* Parser::Array_Variable(string id)
{
    if(CurrentToken->Type==LeftBrackets)
    {
        ConsumeToken();

        ArrayVariableNode* arr=new ArrayVariableNode(Expression_List(),id);
        if(CurrentToken->Type==RightBrackets)
        {
            ConsumeToken();
            return arr;
        }else
        {
            throw ParserException(string("se esperaba ],Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else
    {
        SimpleVariableNode *e =new SimpleVariableNode(id);
        return e;
        //Epsilon
    }
}

list<ExpressionNode *> *Parser::Expression_List()
{
    if(CurrentToken->Type==Id ||CurrentToken->Type==Const_entero||CurrentToken->Type==Op_Sub || CurrentToken->Type==Const_cadena || CurrentToken->Type==Const_caracter || CurrentToken->Type==Const_real ||CurrentToken->Type==verdadero ||CurrentToken->Type==falso ||CurrentToken->Type==LeftParent || CurrentToken->Type==no)
    {
        ExpressionNode* e=Expression();
        list<ExpressionNode*> *ls=new list<ExpressionNode*>();
        ls->push_back(e);
        ls=Expression_Group(ls);
        return ls;
    }else
    {
        throw ParserException(string("se esperaba una expresion,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

list<ExpressionNode *> *Parser::Expression_ListFunctions(string id)
{
    list<ExpressionNode*> *lst=new list<ExpressionNode*>();
    if(CurrentToken->Type==Id ||CurrentToken->Type==Const_entero||CurrentToken->Type==Op_Sub || CurrentToken->Type==Const_cadena || CurrentToken->Type==Const_caracter || CurrentToken->Type==Const_real ||CurrentToken->Type==verdadero ||CurrentToken->Type==falso ||CurrentToken->Type==LeftParent || CurrentToken->Type==no)
    {
        ExpressionNode* e=Expression();

        lst->push_back(e);
        lst=Expression_Group(lst);
        return lst;
    }else
    {
        return lst;//throw ParserException(string("se esperaba una expresion,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

list<ExpressionNode *> *Parser::Expression_Group(list<ExpressionNode *> *ls)
{
    if(CurrentToken->Type==comma)
    {
        ConsumeToken();
        ExpressionNode* e=Expression();
        ls->push_back(e);
        ls=Expression_Group(ls);
        return ls;
    }else
    {
        return ls;//Epsilon
    }
}

StatementNode *Parser::Statement_Mientras()
{
    if(CurrentToken->Type==mientras)
    {
        ConsumeToken();
        ExpressionNode* expr=Expression();
        if(CurrentToken->Type==haga)
        {
            ConsumeToken();
            list<StatementNode*>* ls= new list<StatementNode*>();
            ls=Statement_List(ls);
            if(CurrentToken->Type==fin)
            {
                ConsumeToken();
                if(CurrentToken->Type==mientras)
                {
                    ConsumeToken();
                    StatementMientrasNode* statement=new StatementMientrasNode(expr,ls);
                    return statement;
                }else
                {
                    throw ParserException(string("se esperaba Mientras,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                }
            }else
            {
                throw ParserException(string("se esperaba Fin,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else
        {
            throw ParserException(string("se esperaba Haga,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba Mientras,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Repita()
{
    if(CurrentToken->Type==repita)
    {
        ConsumeToken();
        list<StatementNode*>* ls=new list<StatementNode*>();
        ls=Statement_List(ls);
        if(CurrentToken->Type==hasta)
        {
            ConsumeToken();
            StatementRepitaNode* statement=new StatementRepitaNode(Expression(),ls);
            return statement;
        }else
        {
            throw ParserException(string("se esperaba Hasta,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba Repita,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Llamar()
{
    if(CurrentToken->Type==llamar)
    {
        ConsumeToken();
        if(CurrentToken->Type==Id)
        {
            string id=CurrentToken->Lexeme;
            ConsumeToken();
            if(CurrentToken->Type==LeftParent)
            {
                ConsumeToken();
                list<ExpressionNode*>* ls=Expression_List();
                if(CurrentToken->Type==RightParent)
                {
                    ConsumeToken();
                    StatementLlamarNode* statement=new StatementLlamarNode(id,ls);
                    return statement;
                }else
                {
                    throw ParserException(string("se esperaba un ),Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
                }
            }else
            {
                throw ParserException(string("se esperaba un (,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else
        {
            throw ParserException(string("se esperaba un ID,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba Llamar,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Assignment()
{
    if(CurrentToken->Type==Id)
    {
        VariableNode* var=Variable();
        if(CurrentToken->Type==Op_Assignment)
        {
            ConsumeToken();
            StatementAssignmentNode* statement=new StatementAssignmentNode(var,Expression());
            return statement;
        }else
        {
            throw ParserException(string("se esperaba <-,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }
}

void Parser::Statement_Case()
{
    if(CurrentToken->Type==caso)
    {
        ConsumeToken();
        Variable();
        Case_List();
        Sino_Case();
        if(CurrentToken->Type==fin)
        {
            ConsumeToken();
            if(CurrentToken->Type==caso)
            {
                ConsumeToken();
            }else
            {
                throw ParserException(string("se esperaba Caso,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
            }
        }else
        {
            throw ParserException(string("se esperaba Fin Caso,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else{
        throw ParserException(string("se esperaba Caso,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

void Parser::Case_List()
{
    Define_Case();
    Case_Group();
}

void Parser::Define_Case()
{
    Literal_List();
    if(CurrentToken->Type==colon)
    {
        ConsumeToken();
        Statement_List();
    }else
    {
        throw ParserException(string("se esperaba :,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

void Parser::Case_Group()
{
    if(CurrentToken->Type==Const_entero ||CurrentToken->Type==Const_real || CurrentToken->Type==Const_caracter || CurrentToken->Type==Const_cadena)
    {
        Define_Case();
        Case_Group();
    }else
    {
        //Epsilon
    }
}

void Parser::Literal_List()
{
    Literal();
    Literal_Group();
}

void Parser::Literal_Group()
{
    if(CurrentToken->Type==comma)
    {
        ConsumeToken();
        Literal();
        Literal_Group();
    }else
    {
        //Epsilon
    }
}

void Parser::Literal()
{
    if(CurrentToken->Type==Const_entero ||CurrentToken->Type==Const_real || CurrentToken->Type==Const_caracter || CurrentToken->Type==Const_cadena)
    {
        ConsumeToken();
    }else
    {
        throw ParserException(string("se esperaba una Literal,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

void Parser::Sino_Case()
{
    if(CurrentToken->Type==sino)
    {
        ConsumeToken();
        if(CurrentToken->Type==colon)
        {
            ConsumeToken();
            Statement_List();
        }else
        {
            throw ParserException(string("se esperaba :,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else
    {
        //Epsilon
    }
}

StatementNode *Parser::Statement_Escriba()
{
    if(CurrentToken->Type==escriba)
    {
        ConsumeToken();
        list<ExpressionNode*>*ls=new list<ExpressionNode*>();
        ls=Expression_List();
        StatementEscribaNode* statement=new StatementEscribaNode(ls);
        return statement;
    }else
    {
        throw ParserException(string("se esperaba Escriba,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Abrir_Archivo()
{
    if(CurrentToken->Type==abrir)
    {
        ConsumeToken();
        ExpressionNode* e=Expression();
        if(CurrentToken->Type==como)
        {
            ConsumeToken();
            VariableNode* var=Variable();
            StatementAbrirArchivoNode* statement=new StatementAbrirArchivoNode(e,var,Operation_List());
            return statement;
        }else
        {
            throw ParserException(string("se esperaba Como,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else
    {
        throw ParserException(string("se esperaba Abrir,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

list<string>* Parser::Operation_List()
{
    list<string>* ls=new list<string>();
    if(CurrentToken->Type==para)
    {
        ConsumeToken();

        ls->push_back(Operation());
        string o=Operation_Group();
        if(o.compare("")!=0)
        {
            ls->push_back(o);
        }
        return ls;

    }else
    {
        return ls;//Epsilon
    }
}

string Parser::Operation_Group()
{
    if(CurrentToken->Type==comma)
    {
        ConsumeToken();
        return Operation();

    }else
    {
        return "";//Epsilon
    }
}

string Parser::Operation()
{
    if(CurrentToken->Type==lectura ||CurrentToken->Type==escritura)
    {
        ConsumeToken();
        return Lex->ToLowerCase(CurrentToken->Lexeme);
    }else
    {
        throw ParserException(string("se esperaba Escritura o Lectura,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Cerrar_Archivo()
{
    if(CurrentToken->Type==cerrar)
    {
        ConsumeToken();
        VariableNode* var=Variable();
        StatementCerrarArchivoNode* statement=new StatementCerrarArchivoNode(var);
        return statement;
    }else
    {
        throw ParserException(string("se esperaba Cerrar,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode* Parser::Statement_Escribir_Archivo()
{
    if(CurrentToken->Type==escribir)
    {
        ConsumeToken();
        list<ExpressionNode*>* exprs=new list<ExpressionNode*>();
        ExpressionNode* e=Expression();
        if(CurrentToken->Type==comma)
        {
            ConsumeToken();
            exprs=Expression_List();
        }else
        {
            throw ParserException(string("se esperaba , ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
        exprs->push_back(e);
        StatementEscribirArchivoNode* statement=new StatementEscribirArchivoNode(exprs);
        return statement;
    }else
    {
        throw ParserException(string("se esperaba Escribir,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

StatementNode *Parser::Statement_Leer_Archivo()
{
    if(CurrentToken->Type==leer)
    {
        ConsumeToken();
        list<VariableNode*> *vars=new list<VariableNode*>();
        vars->push_back(Variable());
        if(CurrentToken->Type==comma)
        {
            ConsumeToken();

            vars->push_back(Variable());
            vars=Variable_List(vars);
        }else
        {
            throw ParserException(string("se esperaba , ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
        StatementLeerArchivoNode* statement=new StatementLeerArchivoNode(vars);
        return statement;

    }else
    {
        throw ParserException(string("se esperaba Leer,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

list<VariableNode *> *Parser::Variable_List(list<VariableNode*>* var)
{
    if(CurrentToken->Type==comma)
    {
        ConsumeToken();
        VariableNode* v=Variable();
        var->push_back(v);
        Variable_List(var);
        return var;
    }else
    {
        return var;//Epsilon
    }
}

ExpressionNode* Parser::Expression()
{

    if(CurrentToken->Type==Id ||CurrentToken->Type==Const_entero||CurrentToken->Type==Op_Sub || CurrentToken->Type==Const_cadena || CurrentToken->Type==Const_caracter || CurrentToken->Type==Const_real ||CurrentToken->Type==verdadero ||CurrentToken->Type==falso ||CurrentToken->Type==LeftParent|| CurrentToken->Type==no)
    {
        ExpressionNode* e=Bool_Expression();
        return ExpressionP(e);
    }else
    {
        throw ParserException(string("se esperaba una expresion,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}
ExpressionNode *Parser::ExpressionP(ExpressionNode *node)
{
    if(CurrentToken->Type==Op_LogicalY)
    {
        ConsumeToken();
        ExpressionNode* e=Bool_Expression();
        LogicalYNode* log= new LogicalYNode(node,e);
        return ExpressionP(log);
    }else if(CurrentToken->Type==Op_LogicalO)
    {
        ConsumeToken();
        ExpressionNode* e=Bool_Expression();
        LogicalONode* logo=new LogicalONode(node,e);
        return ExpressionP(logo);
    }else
    {
        return node;//Epsilon
    }
}

ExpressionNode *Parser::Bool_Expression()
{
    ExpressionNode* e=Basic_Expression();
    return Bool_ExpressionP(e);
}

ExpressionNode *Parser::Bool_ExpressionP(ExpressionNode* node)
{
    if(CurrentToken->Type==Op_LessThan)
    {
        ConsumeToken();
        ExpressionNode* e=Basic_Expression();
        LessThanNode* less=new LessThanNode(node,e);
        return Bool_ExpressionP(less);
    }else if(CurrentToken->Type==Op_GreaterThan)
    {
        ConsumeToken();
        ExpressionNode* e=Basic_Expression();
        GreaterThanNode* greater=new GreaterThanNode(node,e);
        return Bool_ExpressionP(greater);
    }else if(CurrentToken->Type==Op_LessOrEqualThan)
    {
        ConsumeToken();
        ExpressionNode* e=Basic_Expression();
        LessAndEqualThanNode* lesE=new LessAndEqualThanNode(node,e);
        return Bool_ExpressionP(lesE);
    }else if(CurrentToken->Type==Op_GreaterOrEqualThan)
    {
        ConsumeToken();
        ExpressionNode* e=Basic_Expression();
        GreaterAndEqualThanNode* greE=new GreaterAndEqualThanNode(node,e);
        return Bool_ExpressionP(greE);
    }else if(CurrentToken->Type==Equal)
    {
        ConsumeToken();
        ExpressionNode* e=Basic_Expression();
        EqualNode* equal=new EqualNode(node,e);
        return Bool_ExpressionP(equal);
    }else if(CurrentToken->Type==Op_NotEqual)
    {
        ConsumeToken();
        ExpressionNode* e=Basic_Expression();
        NotEqualNode* nEqual=new NotEqualNode(node,e);
        return Bool_ExpressionP(nEqual);
    }else
    {
        return node;//Epsilon
    }
}

ExpressionNode *Parser::Basic_Expression()
{
    ExpressionNode* e=Factor();
    return Basic_ExpressionP(e);
}

ExpressionNode *Parser::Basic_ExpressionP(ExpressionNode *node)
{
    if(CurrentToken->Type==Op_Sum)
    {
        ConsumeToken();
        ExpressionNode* e=Factor();
        SumNode* sum=new SumNode(node,e);
        return Basic_ExpressionP(sum);
    }else if(CurrentToken->Type==Op_Sub)
    {
        ConsumeToken();
        ExpressionNode* e=Factor();
        SubtractionNode* sub=new SubtractionNode(node,e);
        return Basic_ExpressionP(sub);
    }else
    {
        return node;//Epsilon
    }
}

ExpressionNode *Parser::Factor()
{
    ExpressionNode* e=Exp_Op();
    return FactorP(e);
}

ExpressionNode *Parser::FactorP(ExpressionNode *node)
{
    if(CurrentToken->Type==Op_Mult)
    {
        ConsumeToken();
        ExpressionNode* e=Exp_Op();
        MultiplicationNode* mult=new MultiplicationNode(node,e);
        return FactorP(mult);
    }else if(CurrentToken->Type==Op_Div)
    {
        ConsumeToken();
        ExpressionNode* e=Exp_Op();
        DivisionNode* div=new DivisionNode(node,e);
        return FactorP(div);
    }else if(CurrentToken->Type==Div)
    {
        ConsumeToken();
        ExpressionNode* e=Exp_Op();
        IntegerDivisionNode* idiv=new IntegerDivisionNode(node,e);
        return FactorP(idiv);
    }else if(CurrentToken->Type==mod)
    {
        ConsumeToken();
        ExpressionNode* e=Exp_Op();
        ModNode* mod_node=new ModNode(node,e);
        return FactorP(mod_node);
    }else
    {
        return node;//Epsilon
    }
}

ExpressionNode *Parser::Exp_Op()
{
    ExpressionNode* e=LogicalNot();
    return Exp_OpP(e);
}

ExpressionNode *Parser::Exp_OpP(ExpressionNode *node)
{
    if(CurrentToken->Type==Op_Exp)
    {
        ConsumeToken();
        ExpressionNode* e=LogicalNot();
        ExponentialNode* exp=new ExponentialNode(node,e);
        return Exp_OpP(exp);
    }else
    {
        return node;//Epsilon
    }
}

ExpressionNode *Parser::LogicalNot()
{
    if(CurrentToken->Type==no)
    {
        ConsumeToken();
        LogicalNotNode* ln=new LogicalNotNode(Term());
        return ln;
    }else if(CurrentToken->Type==Id || CurrentToken->Type==Const_entero ||CurrentToken->Type==Op_Sub || CurrentToken->Type==Const_cadena || CurrentToken->Type==Const_caracter || CurrentToken->Type==Const_real || CurrentToken->Type==verdadero || CurrentToken->Type==falso || CurrentToken->Type==LeftParent)
    {
        return Term();
    }else
    {
        throw ParserException(string("se esperaba un termino,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}


ExpressionNode *Parser::Term()
{
    if(CurrentToken->Type==Id)
    {
        string id=CurrentToken->Lexeme;
        ConsumeToken();
        return Id_Term(id);
    }else if(CurrentToken->Type==LeftParent)
    {
        ConsumeToken();
        //Expression();

        list<ExpressionNode*> *ls=Expression_List();
        ExpressionGroupNode *E_group=new ExpressionGroupNode(ls);
        if(CurrentToken->Type==RightParent)
        {
            ConsumeToken();
            return E_group;
        }else
        {
            throw ParserException(string("se esperaba ) ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else if(CurrentToken->Type==Const_entero || CurrentToken->Type==Const_cadena || CurrentToken->Type==Const_caracter || CurrentToken->Type==Const_real || CurrentToken->Type==verdadero || CurrentToken->Type==falso)
    {
        ExpressionNode *e;
        if(CurrentToken->Type==Const_entero)
        {
            e=new EnteroNode(atoi(CurrentToken->Lexeme.c_str()));
        }else if(CurrentToken->Type==Const_cadena)
        {
            e=new CadenaNode(CurrentToken->Lexeme);
        }else if(CurrentToken->Type==Const_caracter)
        {
            e=new CaracterNode(CurrentToken->Lexeme.at(0));
        }else if(CurrentToken->Type==Const_real)
        {
            e=new RealNode(atof(CurrentToken->Lexeme.c_str()));
        }else if(CurrentToken->Type==verdadero || CurrentToken->Type==falso)
        {
            e=new BoolNode(CurrentToken->Lexeme);
        }
        ConsumeToken();
        return e;
    }else if(CurrentToken->Type==Op_Sub)
    {
        ConsumeToken();
        return Const_Negative();
    }else
    {
        throw ParserException(string("se esperaba un Factor,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

ExpressionNode *Parser::Id_Term(string id)
{
    if(CurrentToken->Type==LeftParent)
    {
        ConsumeToken();
        list<ExpressionNode*> *ls=Expression_ListFunctions(id);
        ExpressionFunctionNode *e= new ExpressionFunctionNode(id,ls);
        if(CurrentToken->Type==RightParent)
        {
            ConsumeToken();
            return e;
        }else
        {
            throw ParserException(string("se esperaba ) ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else if(CurrentToken->Type==LeftBrackets || CurrentToken->Type==dot)
    {
        return Variable_Factor(id);
    }else
    {
        SimpleVariableNode *id_node=new SimpleVariableNode(id);
        return id_node;//Epsilon
    }
}

ExpressionNode *Parser::Variable_Factor(string id)
{
    VariableNode* var1=Array_Variable(id);
    return Compuest_Variable(var1);

}

ExpressionNode *Parser::Const_Negative()
{
    if(CurrentToken->Type==Const_entero || CurrentToken->Type==Const_real || CurrentToken->Type==Id)
    {
        ExpressionNode* e;
        if(CurrentToken->Type==Const_entero)
        {
            e=new EnteroNode(atoi(CurrentToken->Lexeme.c_str()));
        }else if(CurrentToken->Type==Const_real)
        {
            e=new RealNode(atof(CurrentToken->Lexeme.c_str()));
        }else
        {
            e=new SimpleVariableNode(CurrentToken->Lexeme);
        }
        ConsumeToken();
        return e;
    }else if(CurrentToken->Type==LeftParent)
    {
        ConsumeToken();
        ExpressionGroupNode* eg=new ExpressionGroupNode(Expression_List());
        if(CurrentToken->Type==RightParent)
        {
            ConsumeToken();
            return eg;
        }else
        {
            throw ParserException(string("Se esperaba ) ,real, un Id o un ( ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
        }
    }else
    {
        throw ParserException(string("Tipos de datos incompatibles,real, un Id o un ( ,Fila:")+to_string(CurrentToken->Row)+",Columna:"+to_string(CurrentToken->Column));
    }
}

