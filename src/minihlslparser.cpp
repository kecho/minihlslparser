#include "minihlslparser.h"
#include "stdio.h"
#define PARSE_ERROR(str) printf("%s\n",str);mIsError=true;
#define NEXT_TOKEN_NE() NextToken();if(mCurrToken>=mTokenSize){printf("Unexpected EOF\n");return 0;}
void * MemBlock::Allocate(int bytes)
{
    void * ret = 0;
    if (mCurrOffset + bytes < sSize)
    {
        ret =  &mBlock[mCurrOffset];
        mCurrOffset += bytes; 
    }
    return ret;
}

void MemBlock::Reset()
{
    mCurrOffset = 0;
}

template<typename T> T * GeneralAllocate(MemBlock& block)
{
    T * p = reinterpret_cast<T*>(block.Allocate(sizeof(T)));
    if (p == 0) printf("ERROR: RAN OUT OF MEMORY");
    return p;
}

MiniHlslParser::Mixed * MiniHlslParser::NewMixed()
{
    return GeneralAllocate<MiniHlslParser::Mixed>(mRawMemory);
}

MiniHlslParser::StructT * MiniHlslParser::NewStructT()
{
    return GeneralAllocate<MiniHlslParser::StructT>(mRawMemory);
}

MiniHlslParser::NodeList * MiniHlslParser::NewNodeList()
{
    return GeneralAllocate<MiniHlslParser::NodeList>(mRawMemory);
}

MiniHlslParser::Variable * MiniHlslParser::NewVariable()
{
    return GeneralAllocate<MiniHlslParser::Variable>(mRawMemory);
}

MiniHlslParser::ScalarT * MiniHlslParser::NewScalarT()
{
    return GeneralAllocate<MiniHlslParser::ScalarT>(mRawMemory);
}

MiniHlslParser::ArrayT * MiniHlslParser::NewArrayT()
{
    return GeneralAllocate<MiniHlslParser::ArrayT>(mRawMemory);
}

int getint(const char * str, int len)
{
    int d = 0;
    for (int i = 0; i < len; ++i) d = d*10 + (str[i] - '0');
    return d;
}

MiniHlslParser::Variable * MiniHlslParser::GetVariable()
{
    MiniHlslParser::Mixed * m = GetType();
    if (m)
    {
        MiniHlslParser::Token * t = NEXT_TOKEN_NE();
        MiniHlslParser::Variable * v = NewVariable();
        if (t->mType == MiniHlslParser::IDENTIFIER)
        {
            v->identifier = t; 
            t = NEXT_TOKEN_NE();
        }
        else
        {
            v->identifier = 0;
        }

        while (t->mType == MiniHlslParser::LEFT_ARR_BRAC)
        {
            t = NEXT_TOKEN_NE();
            if (t->mType == MiniHlslParser::IMM)
            {
                int arrSize = getint(t->mStart, t->mStrLen);
                ArrayT * arrT = NewArrayT();
                arrT->size = arrSize;
                arrT->mixed = m;
                MiniHlslParser::Mixed * wrapper = NewMixed();
                wrapper->pointer.arrayT = arrT;
                wrapper->type = MiniHlslParser::ARRAY_T;
                m = wrapper;
                t = NEXT_TOKEN_NE();
                if (t->mType != MiniHlslParser::RIGHT_ARR_BRAC)
                {
                    PARSE_ERROR("Right bracket expected");
                }
                t = NEXT_TOKEN_NE();
            }
            else
            {
                PARSE_ERROR("Imm value expected");
            }
        }

        if (t->mType == MiniHlslParser::SEMICOLON)
        {
            v->type = m;
            return v;
        }
        else
        {
            PARSE_ERROR("Semicolon expected");
            return 0;
        }
    }
    return 0;
}

MiniHlslParser::ScalarT * MiniHlslParser::GetScalarT()
{
    MiniHlslParser::ScalarT * s = NewScalarT();
    MiniHlslParser::Token * t = NEXT_TOKEN_NE();
    s->identifier = t; 
    if (t->mType == MiniHlslParser::FLOATN)
    {
        s->type = MiniHlslParser::FLOAT_I;
    }
    else if (t->mType == MiniHlslParser::INTN)
    {
        s->type = MiniHlslParser::INT_I;
    }
    else if (t->mType == MiniHlslParser::BOOLN)
    {
        s->type = MiniHlslParser::BOOL_I;
    }
    else
    {
        PrevToken();
        return 0;
    }
    char lastChar = s->identifier->mStart[s->identifier->mStrLen - 1];
    s->dimensions = lastChar >= '0' && lastChar <= '4' ? lastChar - '0' : 1;
    return s;
}

MiniHlslParser::NodeList * MiniHlslParser::GetNodeList()
{
    MiniHlslParser::NodeList * nodeList = 0;
    MiniHlslParser::Token * t = NEXT_TOKEN_NE();
    if (t->mType != MiniHlslParser::RIGHT_BRAC)
    {
        PrevToken();
        nodeList = NewNodeList(); 
        nodeList->data = GetVariable();
        nodeList->next = GetNodeList();
    }
    return nodeList;
}

MiniHlslParser::StructT * MiniHlslParser::GetStructT()
{
    MiniHlslParser::Token * t = NEXT_TOKEN_NE();
    if (t->mType == MiniHlslParser::STRUCT)
    {
        MiniHlslParser::StructT * s = NewStructT();  
        t = NEXT_TOKEN_NE();
        if (t->mType == MiniHlslParser::IDENTIFIER)
        {
            s->identifier = t;
            t = NEXT_TOKEN_NE();
        }
        else
        {
            s->identifier = 0;
        }
        

        if (t->mType == MiniHlslParser::LEFT_BRAC)
        {
            s->members = GetNodeList();
        }
        else
        {
            PARSE_ERROR("Error parsing struct token");
        }
        return s;
    }
    PrevToken();//backtrack
    return 0;
}

MiniHlslParser::Mixed * MiniHlslParser::GetType()
{
    MiniHlslParser::Mixed * m = NewMixed();
    m->type = MiniHlslParser::INVALID_T;

    if ((m->pointer.structT = GetStructT()))
    {
        m->type = MiniHlslParser::STRUCT_T;
    }
    else if (m->pointer.scalarT = GetScalarT())
    {
        m->type = MiniHlslParser::SCALAR_T;
    } 
    else
    {
        PARSE_ERROR("ERROR PARSING STRUCT, type expected");
    }

    return m;
}

void MiniHlslParser::Parse(const char * str)
{
    Reset();
    Tokenize(str);
    mRoot = GetType();
}

void MiniHlslParser::Reset()
{
    mTokenSize = 0;
    mCurrToken = -1;
    mRawMemory.Reset();
}

int IsStr(const char * str, const char * comparison, int comparisonLen)
{
    int counter = 0;
    if (*str)
    {
        for (;counter < comparisonLen; ++counter)
        {
            if (str[counter] != comparison[counter])
                return -1;
        }
    }
    return counter == 0 ? -1 : counter;
}

int IsStruct(const char * str)
{
    return IsStr(str, "struct", 6);
}

int IsStrN(const char * str, const char * comparison, int comparisonLen)
{
    int ret = IsStr(str, comparison, comparisonLen);
    if (ret != -1 && (str[ret] >= '1' && str[ret] <= '4'))
    {
        ++ret;
    }
    return ret;
}

int IsFloatN(const char * str)
{
    return IsStrN(str, "float", 5);
}

int IsIntN(const char * str)
{
    int len = IsStrN(str, "int", 3);
    return len;
}

int IsBoolN(const char * str)
{
    return IsStrN(str, "bool", 4);
}

int IsLeftBrac(const char * str)
{
    return *str && *str == '{' ? 1 : -1;
}

int IsRightBrac(const char * str)
{
    return *str && *str == '}' ? 1 : -1;
}

int IsSemicolon(const char * str)
{
    return *str && *str == ';' ? 1 : -1;
}

int IsLeftArrBrac(const char * str)
{
    return *str && *str == '[' ? 1 : -1;
}

int IsRightArrBrac(const char * str)
{
    return *str && *str == ']' ? 1 : -1;
}
#define IS_LETTER(X) ((X >= 'a' && X <='z') || (X >= 'A' && X <= 'Z'))
#define IS_NUMBER(X) (X >= '0' && X <= '9')

int IsIdentifier(const char * str)
{
    if (*str && IS_LETTER(*str) )
    {
        int counter = 0;
        while (IS_LETTER(*str) || IS_NUMBER(*str)) {++counter; ++str;}
        return counter;
    }
    return -1;
}


int IsBlank(const char * str)
{
    if (*str)
    {
        int counter = 0;
        while(*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r' )
        {
            ++str;
            ++counter;
        }
        return counter == 0 ? -1 : counter;
    }
    return -1;
}

int IsImm(const char* str)
{
    const char * s = str;
    int len = 0;
    while(*s && IsBlank(s) == -1)
    {
        if (*s < '0' || *s > '9')
        {
            return len == 0 ? -1 : len; 
        }
        len++;
        s++;
    }
    return len == 0 ? -1 : len;
}

const char * MiniHlslParser::Consume(const char * str, MiniHlslParser::TokenType type, int length)
{
    if (length == -1) return str;
    if (type != BLANK)
    {
        Token * t = &mTokens[mTokenSize++]; 
        t->mStart = str;
        t->mStrLen = length;
        t->mType = type;
    }
    return str+length;
}

void MiniHlslParser::Tokenize(const char * str)
{
    const char * ptr = str;
    while (*ptr)
    {
        int len = 0;
        if ( ( len = IsBlank(ptr) ) != -1)
        {
            ptr = Consume(ptr, BLANK, len);
        }
        else if ( (len = IsStruct(ptr))  != -1)
        {
            ptr = Consume(ptr,STRUCT,len);
        } 
        else if ( (len = IsLeftBrac(ptr)) != -1)
        {
            ptr = Consume(ptr, LEFT_BRAC, len);
        }
        else if ( (len = IsRightBrac(ptr)) != -1)
        {
            ptr = Consume(ptr, RIGHT_BRAC, len);
        }
        else if ( (len = IsLeftArrBrac(ptr)) != -1)
        {
            ptr = Consume(ptr, LEFT_ARR_BRAC, len);
        }
        else if ( (len = IsRightArrBrac(ptr)) != -1)
        {
            ptr = Consume(ptr, RIGHT_ARR_BRAC, len);
        }
        else if ( (len = IsSemicolon(ptr)) != -1)
        {
            ptr = Consume(ptr, SEMICOLON, len);
        }
        else if ( (len = IsImm(ptr)) != -1 )
        {
            ptr = Consume(ptr, IMM, len);
        }
        else if ( (len = IsIdentifier(ptr)) != -1)
        {
            if ( (len == IsFloatN(ptr)))
            {
                ptr = Consume(ptr, FLOATN, len);
            }
            else if ( (len == IsIntN(ptr)))
            {
                ptr = Consume(ptr, INTN, len);
            }
            else if ( (len == IsBoolN(ptr)))
            {
                ptr = Consume(ptr, BOOLN, len);
            }
            else
            {
                ptr = Consume(ptr, IDENTIFIER, len);
            }
        }

        if (len == -1) return;
    }
}

void copyStr(const char * src, char * dest, int len)
{
    dest[len]='\0';
    --len;
    while(len>=0)
    {
        dest[len] = src[len];
        --len;
    }
}

void MiniHlslParser::PrintTokens()
{
    char buff[100];
    for (int i = 0; i < mTokenSize; ++i)
    {
        switch (mTokens[i].mType)
        {
            case STRUCT:
                printf("STRUC ");
                break;
            case LEFT_BRAC:
                printf("LEFT_BRAC ");
                break;
            case RIGHT_BRAC:
                printf("RIGHT_BRAC ");
                break;
            case LEFT_ARR_BRAC:
                printf("LEFT_ARR_BRAC");
                break;
            case RIGHT_ARR_BRAC:
                printf("RIGHT_ARR_BRAC");
                break;
            case IMM:
                printf("IMM");
                break;
            case SEMICOLON:
                printf("SEMICOLON ");
                break;
            case IDENTIFIER:
                printf("IDENTIFIER ");
                break;
            case FLOATN:
                printf("FLOATN ");
                break;
            case INTN:
                printf("INTN ");
                break;
            case BOOLN:
                printf("BOOLN ");
                break;
        }
        copyStr(mTokens[i].mStart, buff, mTokens[i].mStrLen);
        printf("\"%s\" \n",buff);
    }
}
