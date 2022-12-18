#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// #define DEBUG_ON

typedef enum{
    TK_RESERVED, // anything but number. + or - or == and else
    TK_NUM,
    TK_EOF, //EOF end of file
    TK_IDENT,
    TK_RETURN
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

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};
LVar *find_lvar(Token *tok);
extern LVar *locals;

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
    ND_LE, //<=
    ND_ASSIGN, //=
    ND_LVAR, // local variable
    ND_RETURN,
    ND_IF,
    ND_IFELSE// if, while, for
} NodeKind;

typedef struct Node Node;

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

extern Node *code[100];


struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

Node *new_node_num(int val);

void gen(Node *node);
