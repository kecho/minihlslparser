#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT
#include "minihlslparser.h"

class PrettyPrint : public MiniHlslParser::Visitor
{
public:
    PrettyPrint();
    virtual ~PrettyPrint();
    virtual void Visit(MiniHlslParser::Mixed * m);
    virtual void Visit(MiniHlslParser::ScalarT * m);
    virtual void Visit(MiniHlslParser::StructT * m);
    virtual void Visit(MiniHlslParser::ArrayT * m);
    virtual void Visit(MiniHlslParser::NodeList * m);
    virtual void Visit(MiniHlslParser::Variable * m);
private:
    void Indent();
    void out(const char * s);
    void outnl(const char * s);
    void cpstr(const char * src, char * dst, int len);
    void out (MiniHlslParser::Token * identifier);
    void recursePrintArray(MiniHlslParser::ArrayT *  a);
    int indent;
};
#endif
