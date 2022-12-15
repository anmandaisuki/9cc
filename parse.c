#include "9cc.h"


Token *token; //global type. Used in several function or main()

char *user_input;

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos: the number of " "
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char *op){
    if(token->kind != TK_RESERVED ||
     token->len != strlen(op)||
     memcmp(token->str,op,token->len))
        return false;

    token = token-> next;
    return true;
}

Token* consume_ident(){
    Token* r_token;
    if(token->kind == TK_IDENT){
        r_token = token;
        token = token->next;
        return r_token;
    }
    return NULL;
}

void expect(char *op){
    if(token->kind != TK_RESERVED || 
    token->len != strlen(op)||
     memcmp(token->str,op,token->len))
    {
        // error("this is not '%c'", op);
        error_at(token->str,"this is not '%c",op);
    }
    token = token->next;
}

int expect_number(){
    if(token->kind != TK_NUM){
        // error("this is not number");
        error_at(token->str,"this is not number");
    }
    int val= token->val;
    token = token->next; // move to next token
    return val;
}

bool at_eof(){
    return token->kind ==TK_EOF;
}

//cur is initialized area
// cur: initilezed memory and filled inside of new_token()
// tok is reserved memory area handed to cur.   
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token)); // reserve next token memory
    tok->kind = kind;
    tok->str = str; 
    tok->len = len;
    cur->next = tok;
    return tok;
}

//1 is correspond *p=*q
bool startwith(char *p, char *q){
    return memcmp(p,q,strlen(q)) == 0; // reverse result 
}

// *p is input code
Token *tokenize(char *p){

    //
    Token head;
    head.next = NULL;

    //cur is pointer of current token
    // cur is updated by new_token()
    Token *cur = &head;

    //cur is always 0 (initialized 0 by calloc()) and filled by new_token() and cur is moved to next blank space() at the same time.
    while(*p){
        // skip space
        if(isspace(*p)){
            p++;
            continue;
        }

        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';'){

            cur = new_token(TK_RESERVED, cur, p++,1); // p++: incriment is done after procedure. ++p : incriment is done before procedure
            
            continue;
        }

        if('a' <= *p && *p <= 'z'){
            cur=new_token(TK_IDENT,cur,p++,1);
            continue;
        }


        if(startwith(p,"==") || startwith(p, "!=") || startwith(p,"<=") || startwith(p, ">=")){

            cur = new_token(TK_RESERVED, cur, p,2); // p++: incriment is done after procedure. ++p : incriment is done before procedure
            p+=2; // pointer is incremented 2
            

            continue;
        }

        // check number or not
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p,0);
            char *q = p;
            cur->val = strtol(p, &p, 10); //p is treated as string not char. So &p is regarded as string pointer. 10 is decimal
            cur->len = p-q;
            // p is updated to next token. 
            // every input is char. 12 -> "1" "2" (ascii code) 0x51 0x52
            // strtol is translate ascii char to number
            continue;
        }

        // error(" tokenize failed");
        error_at(p,"tokenize process is failed. Code is not correct.\n Check the code indicated by ^ \n");
    }

    new_token(TK_EOF, cur, p,0);
    return head.next;
}


//proto type 
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

Node *code[100];


Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

void program(){
    int i = 0;

    while(!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt(){
    Node *node =expr();
    expect(";");
    return node;
}

Node *expr(){
    // Node *node = mul();
    return assign();

    // for(;;){
    //     if(consume('+'))
    //         node = new_node(ND_ADD, node, mul());
    //     else if(consume('-'))
    //         node = new_node(ND_SUB, node, mul());
    //     else
    //         return node;
    // }
}

Node *assign(){
    Node *node = equality();

    if(consume("="))
        new_node(ND_ASSIGN,node,assign());

    return node;
}

Node *equality(){
    Node *node = relational();

    for(;;){
        if(consume("=="))
            node = new_node(ND_EQ,node,relational());
        else if(consume("!="))
            node = new_node(ND_NE,node,relational());
        else 
            return node;
    }
}

Node *relational(){
    Node *node = add();

    for(;;){
        if(consume("<") )
            node = new_node(ND_LT, node,add());
        if(consume(">") )
            node = new_node(ND_LT,add(), node);
        if(consume("<=") )
            node = new_node(ND_LE, node,add());
        if(consume(">="))
            node = new_node(ND_LE, add(),node);
        else
            return node;
    }
}

Node *add(){
    Node *node = mul();

    for(;;){
        if(consume("+"))
            node = new_node(ND_ADD,node,mul());
        else if(consume("-"))
            node = new_node(ND_SUB,node,mul());
        else 
            return node;
    }

}

Node *mul(){
    // Node *node = primary();
    Node *node = unary();

    for(;;){
        if(consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary(){

    if(consume("+"))
        return primary();
    if(consume("-"))
        return new_node(ND_SUB, new_node_num(0),primary()); // 0 - primary()
    return primary();
}

Node *primary(){
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();

    if(tok){ //if(tok != NULL)
        Node *node = calloc(1,sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }

    return new_node_num(expect_number());
}


