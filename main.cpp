#include <cstdio>
#include <cstdlib>

#include <vector>

enum TokenType
{
    TOKEN_LITERAL,

    TOKEN_OPEN_PARENTHESIS,
    TOKEN_CLOSE_PARENTHESIS,
    
    TOKEN_IMPLICATION,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    
    TOKEN_UNKNOWN
};

struct Token
{
    const char* text;
    size_t length;
    TokenType type;
};

struct StringParser
{
    const char* str;
    size_t at;
    
    StringParser(const char* str)
    {
	this->str = str;
	this->at  = 0;
    }

    Token next()
    {
	Token token;

	char c = str[at];
	if(c >= '0' && c <= '9')
	{
	    token = parse_number();
	}
	else if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
	{
	    token = parse_literal();
	}
	else if(c == '&')
	{
	    token.str = str + at;
	    token.length = 1;
	    token.type = TOKEN_AND;

	    at++;
	}
	else if(c == '|')
	{
	    token.str = str + at;
	    token.length = 1;
	    token.type = TOKEN_OR;

	    at++;
	}
	else if(c == '~')
	{
	    token.str = str + at;
	    token.length = 1;
	    token.type = TOKEN_NOT;

	    at++;
	}
	else if(c == '>')
	{
	    token.str = str + at;
	    token.length = 1;
	    token.type = TOKEN_IMPLICATION;

	    at++;
	}
	else if(c == '(' || c == '[' || c == '{')
	{
	    token.str = str + at;
	    token.length = 1;
	    token.type = TOKEN_OPEN_PARENTHESIS;

	    at++;
	}
	else if(c == ')' || c == ']' || c == '}')
	{
	    token.str = str + at;
	    token.length = 1;
	    token.type = TOKEN_CLOSE_PARENTHESIS;

	    at++;
	}
	
	/*
	switch(str[at])
	{
	case 1: case 2: case 3: case 4: case 5:
	case 6: case 7: case 8: case 9: case 0:
	{
	    token = parseNumber();
	} break;
	
	default:
	{
	    token.str = str + at;
	    token.length = 1;
	    token.type = TOKEN_UNKNOWN;

	    at++;
	} break;
	}
	*/

	return token;
    }
};

int main(int argc, char *argv[])
{

}
