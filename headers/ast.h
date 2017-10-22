#ifndef DAVIS_PUTNAM_AST_INCLUDED
#define DAVIS_PUTNAM_AST_INCLUDED

#include <cstdlib>
#include <cstdio>
#include <vector>

namespace ast
{
  enum Type
  {
    AST_ROOT,

    AST_SECTION,
    AST_DATA,

    AST_AND,
    AST_OR,
    AST_NOT,

    AST_NONE,
  };

  struct AST
  {
    static AST* root;

    Type        type;
    const char* data;
    int         length;

    const AST*  parent;

    std::vector<AST>  children;

    AST()
    {
      // @Security Since for now I decided that the pointer to the root AST is a static field,
      //           I won't allow the user to create 2 AST root at the same time.

      if(AST::root)
      {
        printf("Fatal error: attempting to create a second AST tree. This is not allowed.\nFor more information please refer to the class definition in %s\n", __FILE__);
        exit(-1);
      }
      AST::root = this;

      this->type = AST_ROOT;
    }

    AST(const AST* parent)
      : parent{parent}
    {

    }
  };

  AST* AST::root = nullptr;
}

#endif
