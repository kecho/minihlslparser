#include "prettyprint.h"
#include <stdio.h>

PrettyPrint::PrettyPrint():indent(0)
{
}

PrettyPrint::~PrettyPrint()
{
}
    void PrettyPrint::Visit(MiniHlslParser::Mixed * m)
    {
        switch(m->type)
        {
        case MiniHlslParser::STRUCT_T:
            Visit(m->pointer.structT);
            break;
        case MiniHlslParser::SCALAR_T:
            Visit(m->pointer.scalarT);
            break;
        case MiniHlslParser::ARRAY_T:
            Visit(m->pointer.arrayT);
            break;
        }
    }
    void PrettyPrint::Visit(MiniHlslParser::ScalarT * m) {
        char buff[50]; 
        Indent();
        switch(m->type)
        {
        case MiniHlslParser::FLOAT_I:
            cpstr(m->identifier->mStart, buff, m->identifier->mStrLen); 
            break;
        case MiniHlslParser::INT_I:
            cpstr(m->identifier->mStart, buff, m->identifier->mStrLen); 
            break;
        case MiniHlslParser::BOOL_I:
            cpstr(m->identifier->mStart, buff, m->identifier->mStrLen); 
            break;
        }
        printf("%s",buff);

    }
    void PrettyPrint::Visit(MiniHlslParser::StructT * m) {
        if (m->identifier)
        {
            Indent();
            printf("struct ");
            out(m->identifier);
            printf(" {\n");
        }
        else
        {
            outnl("struct {");
        }
        indent++;
        Visit(m->members);
        indent--;
        out("}");
    }
    void PrettyPrint::Visit(MiniHlslParser::ArrayT * m)
    {
        Visit(m->mixed);
    } 
    void PrettyPrint::Visit(MiniHlslParser::NodeList * m)
    {
        if (m)
        {
            Visit(m->data);   
            Visit(m->next);
        }
    } 

    void PrettyPrint::Visit(MiniHlslParser::Variable * m)
    {
        Visit(m->type); 
        if (m->identifier)
        {
            printf(" ");
            out(m->identifier);
        }
        if (m->type->type == MiniHlslParser::ARRAY_T)
        {
            printf("[%d]",m->type->pointer.arrayT->size);
        }
        printf(";\n");
    }

    void PrettyPrint::Indent()
    {
        for (int i = 0; i < indent; ++i) printf("   ");
    }

    void PrettyPrint::out(const char * s)
    {
        Indent();
        printf("%s",s);
    }
    void PrettyPrint::outnl(const char * s)
    {
        Indent();
        printf("%s\n",s);
    }
    void PrettyPrint::cpstr(const char * src, char * dst, int len)
    {
        int i = 0;
        for (; i < len; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }
    void PrettyPrint::out (MiniHlslParser::Token * identifier)
    {
        char buff[400];
        cpstr(identifier->mStart, buff, identifier->mStrLen > 399 ? 399 : identifier->mStrLen);
        printf("%s",buff);
    }

