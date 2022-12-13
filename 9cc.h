#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>



typedef enum{
    TK_RESERVED, // anything but number. + or - 
    TK_NUM,
    TK_EOF, //EOF end of file
    TK_IDENT,
} TokenKind;

typedef struct Token Token; 

struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len; 
};

extern Token *token; 

extern char *user_input;

void error_at(char *loc, char *fmt, ...);

bool consume(char *op);

void expect(char *op);

int expect_number();

bool at_eof();

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

bool startwith(char *p, char *q);

Token *tokenize(char *p);

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ, //==
    ND_NE, //!=
    ND_LT, //<
    ND_LE //<=
} NodeKind;

typedef struct Node Node;

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();


struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

Node *new_node_num(int val);

void gen(Node *node);
