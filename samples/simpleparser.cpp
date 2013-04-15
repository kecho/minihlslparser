#include "minihlslparser.h"
#include "prettyprint.h"
#include "stdio.h"

int main()
{
    PrettyPrint pp;
    MiniHlslParser p;
    const char * s = "struct {int2 p[10]; int2 asf [20];}";
    printf("parsig: %s\n",s);
    
    p.Parse(s);
//    p.PrintTokens();

    pp.Visit(p.GetRoot());
    printf("\n");
}
