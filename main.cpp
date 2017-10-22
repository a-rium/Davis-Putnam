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



struct Literal
{
  const char* text;
  int length;
  bool negated;
};

typedef std::vector<Literal> Clause;

std::vector<Clause> formalize_expression(std::vector<Token> tokens)
{
  std::vector<Clause> clauses;
  Clause clause;

  bool in_parenthesis = false;
  bool negate_next    = false;
  int token_number = 1;
  for(auto& token : tokens)
  {
    // Checking parenthesis
    if(token.type == TOKEN_LITERAL)
    {
      clause.push_back(Literal{token.text, token.length, negate_next});
      negate_next = false;

      if(!in_parenthesis)
      {
        clauses.push_back(clause);
        clause.clear();
      }
    }
    else
    {
      if(negate_next)
      {
        printf("Warning: token %d contains a NOT operator, but the literal at who it was referred was not found\n", token_number);
        negate_next = false;
      }
      if(token.type == TOKEN_OPEN_PARENTHESIS)
      {
        if(in_parenthesis)
        {
          printf("Error at token %d: token contains an open parenthesis, but we were already in one\n", token_number);
          exit(1);
        }

        in_parenthesis = true;
      }
      else if(token.type == TOKEN_CLOSE_PARENTHESIS)
      {
        if(!in_parenthesis)
        {
          printf("Error at token %d: token contains a close parenthesis, but we were not in one\n", token_number);
          exit(1);
        }

        in_parenthesis = false;

        clauses.push_back(clause);
        clause.clear();
      }
      else if(token.type == TOKEN_NOT)
      {
        negate_next = true;
      }
      else if(token.type == TOKEN_AND)
      {
        if(in_parenthesis)
        {
          printf("Error at token %d: found AND token inside of parenthesis, this is not a valid CNF formula\n", token_number);
          exit(1);
        }

        clause.clear();
      }
      else if(token.type == TOKEN_OR)
      {
        if(!in_parenthesis)
        {
          printf("Error at token %d: found OR token outside of parenthesis, this is not a valid CNF formula\n", token_number);
          exit(1);
        }
      }
      else
      {
        printf("Warning at token %d: unsupported token type(raw value: %.*s)\n", token_number, token.length, token.text);
      }
    }

    token_number++;
  }

  return clauses;
}

void print_clauses(const std::vector<Clause>& clauses)
{
  for(int i = 0; i < clauses.size(); i++)
  {
    auto& clause = clauses[i];

    printf("C%d: ", i + 1);
    if(!clause.empty())
    {
      printf("{");

      for(int j = 0; j < clause.size(); j++)
      {
        auto& literal = clause[j];
        if(literal.negated)
        {
          printf("~");
        }
        printf("%.*s", literal.length, literal.text);
        if(j + 1 < clause.size())
        {
          printf(", ");
        }
      }

      printf("}\n");
    }
    else
    {
      printf("[]");
    }
  }
}

void print_clauses_full(const std::vector<Clause> clauses)
{
  printf("{");
  for(int i = 0; i < clauses.size(); i++)
  {
    auto& clause = clauses[i];
    if(!clause.empty())
    {
      printf("{");
      for(int j = 0; j < clause.size(); j++)
      {
        auto& literal = clause[j];
        if(literal.negated)
        {
          printf("~");
        }
        printf("%.*s", literal.length, literal.text);
        if(j + 1 < clause.size())
        {
          printf(", ");
        }
      }
      printf("}");
    }
    else
    {
      printf("[]");
    }

    if(i + 1 < clauses.size())
    {
      printf(", ");
    }
  }
  printf("}");
}

// returns true if formula it satisfiable, false otherwise
// bool davis_putnam(std::vector<Clause> clauses)
// {
//   auto pivot_queue = create_ordered_pivot_queue();
//   const char* pivot
//
//   while(true)
//   {
//     remove_tautologies(clauses);
//
//     for()
//   }
// }

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

  auto clauses = formalize_expression(tokens);

  print_clauses_full(clauses);
  printf("\n");

  return 0;
}
