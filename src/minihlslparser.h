#ifndef __MINI_HLSL_PARSER__
#define __MINI_HLSL_PARSER__

class MemBlock
{
public:
    MemBlock() : mCurrOffset(0) {}
    ~MemBlock(){}
    void * Allocate(int bytes);
    void Reset();
private:
    static const int sSize = 1000;
    char * mBlock[sSize];
    int mCurrOffset;

        
};

class MiniHlslParser
{
public:
    struct Mixed;
    MiniHlslParser() : mIsError(false) {}
    ~MiniHlslParser(){}
    void Parse(const char * str);
    void Tokenize(const char * str);
    void Reset();
    void PrintTokens();
    bool IsError() {return mIsError;}
    struct Mixed * GetRoot() {return mRoot;}

    enum TokenType {
        BLANK,
        STRUCT,
        LEFT_BRAC,
        RIGHT_BRAC,
        LEFT_ARR_BRAC,
        IMM,
        RIGHT_ARR_BRAC,
        SEMICOLON,
        IDENTIFIER,
        FLOATN,
        INTN,
        BOOLN
    };

    enum IntrinsicType
    {
        FLOAT_I, INT_I, BOOL_I, INVALID_I
    };
    
    enum  GeneralType
    {
        STRUCT_T, SCALAR_T, ARRAY_T, INVALID_T
    };

    struct Token
    {
        const char * mStart;
        int mStrLen;
        TokenType mType;
    };
    struct StructT;
    struct ArrayT;
    struct ScalarT;
    struct NodeList;
    struct Variable;
    struct Mixed 
    {
        union p
        {
            StructT * structT;
            ScalarT * scalarT;
            ArrayT  * arrayT;
        } pointer;
        GeneralType type;
    };

    struct ScalarT
    {
        Token * identifier;
        IntrinsicType type;
        int dimensions;
    };

    struct ArrayT
    {
        int size; 
        Mixed * mixed;
    };

    struct StructT
    {
        Token * identifier;
        struct NodeList * members;
    }; 
    
    struct Variable
    {
        Token * identifier;
        Mixed * type;
    };

    struct NodeList
    {
        Variable * data;
        struct NodeList * next;
    };

    class Visitor
    {
    public:
        Visitor(){}
        ~Visitor(){}
        virtual void Visit(Mixed * m) = 0;
        virtual void Visit(ScalarT * m) = 0;
        virtual void Visit(StructT * m) = 0;
        virtual void Visit(ArrayT * m) = 0;
        virtual void Visit(NodeList * m) = 0;
        virtual void Visit(Variable * m) = 0;
    };
private:

    static const int MAX_TOKENS = 100;
    const char * Consume(const char * str, MiniHlslParser::TokenType, int length);


    Token mTokens[MAX_TOKENS];
    int mTokenSize;
    int mCurrToken;

    Token * NextToken() { if(mCurrToken<mTokenSize) return &mTokens[++mCurrToken];  else return 0;}
    void PrevToken() {--mCurrToken;}
    Token * CurrToken() { if (mCurrToken>=0 && mCurrToken < mTokenSize) return &mTokens[mCurrToken]; else return 0;   }



    MemBlock mRawMemory;
    Mixed * mRoot;

    Mixed * GetType();
    StructT * GetStructT();
    NodeList * GetNodeList();
    ScalarT * GetScalarT();
    Variable * GetVariable();
    

    Mixed * NewMixed();
    ArrayT * NewArrayT();
    StructT * NewStructT();
    NodeList * NewNodeList();
    ScalarT * NewScalarT();
    Variable * NewVariable();

    bool mIsError;
};

#endif
