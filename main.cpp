#include "minihlslparser.h"
#include "stdio.h"

int main()
{
    MiniHlslParser p;
    const char * s = "struct {int2 p[10]; int2 asf [20];}";
    printf("parsing: %s\n",s);

    
    p.Parse(s);
    p.PrintTokens();
    p.Print();
}
