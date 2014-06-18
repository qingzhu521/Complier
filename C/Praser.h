#ifndef PRASER_H
#define PRASER_H
#include "Token.h"
#include <stack>

int ll_praser(Node *&h);
int stmt(Node *&cur);
int stmts(Node *&cur);
int block(Node *&cur);
int  ifstmt(Node *&cur);

#endif
