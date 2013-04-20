minihlslparser
==============

Little utility class that parses hlsl structures. Comes with a tree visitor interface that can be used to implement any sort of analysis on
custom hlsl types (structs, cbuffers)

I did this since I want to work on a rendering processing language that will allow lite render calls
and lite c++ hooks, to talk to bigger rendering implementation. Part of this language will support hlsl constructs such as:

struct K
{
   int2 i;
   float4 b[100];
   float3 c[20][49];
   struct T
   {
       in2 t;
   } p[100];
}

and output the abstract syntax tree.
