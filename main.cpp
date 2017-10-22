#include <cstdio>
#include <cstdlib>

#include <vector>

enum TokenType
{
  TOKEN_LITERAL,
  TOKEN_NUMBER,

  TOKEN_OPEN_PARENTHESIS,
  TOKEN_CLOSE_PARENTHESIS,

  TOKEN_IMPLICATION,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_NOT,

  TOKEN_EOS, // EOS: End Of String

  TOKEN_UNKNOWN
};

struct Token
{
  const char* text;
  int         length;
  TokenType   type;
};

struct StringParser
{
  const char* str;
  size_t      at;

  StringParser(const char* str)
  {
    this->str = str;
    this->at  = 0;
  }

  Token next()
  {
    eat_whitespace();

    Token token;

    char c = str[at];
    if(c >= '0' && c <= '9')
    {
     token = parse_number();
    }
    else if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
    {
      token = parse_literal();
    }
    else if(c == '&')
    {
      token.text = str + at;
      token.length = 1;
      token.type = TOKEN_AND;

      at++;
    }
    else if(c == '|')
    {
      token.text = str + at;
      token.length = 1;
      token.type = TOKEN_OR;

      at++;
    }
    else if(c == '~')
    {
      token.text = str + at;
      token.length = 1;
      token.type = TOKEN_NOT;

      at++;
    }
    else if(c == '>')
    {
      token.text = str + at;
      token.length = 1;
      token.type = TOKEN_IMPLICATION;

      at++;
    }
    else if(c == '(' || c == '[' || c == '{')
    {
      token.text = str + at;
      token.length = 1;
      token.type = TOKEN_OPEN_PARENTHESIS;

      at++;
    }
    else if(c == ')' || c == ']' || c == '}')
    {
      token.text = str + at;
      token.length = 1;
      token.type = TOKEN_CLOSE_PARENTHESIS;

      at++;
    }
    else if(c == '\0')
    {
      token.text = str + at;
      token.length = 1;
      token.type = TOKEN_EOS;
    }

    return token;
  }

  void eat_whitespace()
  {
    while(isWhitespace(str[at]))
    {
      at++;
    }
  }

  Token parse_number()
  {
    Token token;
    token.text = str + at;

    while(isNumeric(str[++at]));

    token.length = (str + at) - token.text;
    token.type = TOKEN_NUMBER;

    return token;
  }

  Token parse_literal()
  {
    Token token;
    token.text = str + at;

    while(isLiteral(str[++at]));

    token.length = (str + at) - token.text;
    token.type = TOKEN_LITERAL;

    return token;
  }

  bool isWhitespace(char c)
  {
    return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\r');
  }

  bool isNumeric(char c)
  {
    return (c >= '0' && c <= '9') || (c == '.');
  };

  bool isLiteral(char c)
  {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
  };

};

typedef std::vector<Token>  Clause;
typedef std::vector<Clause> Clauses;

Clauses transform_into_clauses_collection(std::vector<Token> tokens)
{
  bool in_parenthesis = false;

  Clauses clauses;
  Clause clause;
  for(int i = 0; i < tokens.size(); i++)
  {
    auto& token = tokens[i];
    if(token.type == TOKEN_OPEN_PARENTHESIS)
    {
      in_parenthesis = true;
    }
    else if(in_parenthesis && token.type == TOKEN_CLOSE_PARENTHESIS)
    {
      in_parenthesis = false;

      clauses.push_back(clause);
      clause.clear();
    }
    else if(token.type == TOKEN_LITERAL)
    {
      if(in_parenthesis)
      {
        clause.push_back(token);
      }
      else
      {
        clauses.push_back(Clause{token});
      }
    }
    else if(token.type == TOKEN_NOT)
    {
      if(in_parenthesis)
      {
        clause.push_back(token);
        clause.push_back(tokens[i + 1]);
      }
      else
      {
        Clause clause{token, tokens[i + 1]};
        clauses.push_back(clause);
      }

      i++;
    }
  }

  return clauses;
}

int main(int argc, char *argv[])
{
  std::vector<Token> tokens;
  StringParser parser(argv[1]);

  while(true)
  {
    auto token = parser.next();

    if(token.type == TOKEN_EOS)
    {
      break;
    }

    tokens.push_back(token);
  }

  for(auto& token : tokens)
  {
    printf("%.*s", token.length, token.text);
  }

  printf("\n");

  auto clauses = transform_into_clauses_collection(tokens);

  int i = 1;
  int j = 1;
  for(auto& clause : clauses)
  {
    for(auto& token : clause)
    {
      printf("%.*s", token.length, token.text);
    }

    printf("\n");
  }

  return 0;
}
