#include "9cc.h"


Token *token; //global type. Used in several function or main()
LVar *locals;

char *user_input;

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos: the number of " "
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr,"  Input token value is %c", *loc);
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
        if(*op == ';')
            error_at(token->str, "; is expected at the end of line");

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

bool check_ifElse(){
    if(token->kind == TK_RESERVED && memcmp(token->str,"if",strlen("if")) == 0){

        #ifdef DEBUG_ON
        printf("if else is checking....\n");
        #endif

        Token *search_token;
        search_token = token;
        for(;;){
            search_token = search_token->next;
            if(memcmp(search_token->str,"else",strlen("else")) == 0)
                return true;
            if(memcmp(search_token->str,"if",strlen("if")) == 0)
                return false;
            if(search_token->next == NULL)
                return false;
        }
    }

    #ifdef DEBUG_ON
    printf("if else is not found....\n");
    #endif

    return false;
    //printf("if is not found\n");
    //error_at(token->str,"check_ifElse() is failed \n");
}

bool at_eof(){
    return token->kind ==TK_EOF;
}

//cur is initialized area. Return next token pointer.
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

// lval is checked if lval is appeared already or not.(Exist in locals or not) If lval is new, NULL is returned.
int lval_judge(char *p){
    int n = 0;

    while('a' <= *p && *p <= 'z') {
        n++;
        p++;
    }

    return n;
}

LVar *find_lvar(Token *tok){
    for (LVar *var = locals; var; var = var->next)
        if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL; 
}

// to check token specific token word. ex) right -> return; wrong -> returnx 
int is_alnum(char c){
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
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

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])){
            cur = new_token(TK_RETURN,cur,p,6);
            p += 6;
            continue;
        }

        if(startwith(p,"if")){
            cur = new_token(TK_RESERVED, cur, p,2); 
            p+=2; 
            continue;
        }
        if(startwith(p, "else")){
            cur = new_token(TK_RESERVED, cur, p,4); 
            p+=4; 
            continue;
        }
        if(startwith(p,"while") ){
            cur = new_token(TK_RESERVED, cur, p,5); 
            p+=5; 
            continue;
        }
        if(startwith(p, "for")){
            cur = new_token(TK_RESERVED, cur, p,3); 
            p+=3; 
            continue;
        }


        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';'){

            cur = new_token(TK_RESERVED, cur, p++,1); // p++: incriment is done after procedure. ++p : incriment is done before procedure
            
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

        if('a' <= *p && *p <= 'z'){

            if(lval_judge(p)){
                int n = lval_judge(p);
                cur=new_token(TK_IDENT,cur,p,n);
                p+=n;
                //printf("lval word is detected as %d words\n", n);
                continue;
            }

            cur=new_token(TK_IDENT,cur,p++,1);
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

    #ifdef DEBUG_ON
    printf(" stmt process\n");
    #endif

    Node *node;
    if(token->kind == TK_RETURN){
        token = token->next;
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr(); // expr()>...num node retuned
    }else{
        //node = expr();
        if(check_ifElse()){
            if(!consume("if"))
                error_at(token->str," if is failed ");
            if(!consume("("))
                error_at(token->str," ( is not found after 'if' ");
            
            node = calloc(1,sizeof(Node));
            node->kind = ND_IFELSE;
            node->lhs = expr();
            
            if(!consume(")"))
                error_at(token->str," ) is not found after 'if' ");

            node->rhs = stmt();

            if(consume("else"))
                node = new_node(ND_IFELSE,node, stmt());

            return node;
        }

        if(consume("if")){
            if(!consume("("))
                error_at(token->str," () is not found after 'if' ");
            
            node = calloc(1,sizeof(Node));
            node->kind = ND_IF;
            node->lhs = expr();

            if(!consume(")"))
                error_at(token->str," () is not found after 'if' ");

            node->rhs = stmt();

            return node;
        }

        if(consume("while")){
            if(!consume("(")){
                error_at(token->str," ( is not found after 'while' ");
            }

            node = calloc(1,sizeof(Node));
            node->kind = ND_WHILE;
            node->lhs = expr();

            if(!consume(")"))
                error_at(token->str," ) is not found after 'while' ");
            
            node->rhs = stmt();

            return node;
        }

        if(consume("for")){  // for(A;B;C)D 
            if(!consume("(")){
                error_at(token->str," () is not found after 'for' ");
            }

            node = calloc(1,sizeof(Node));
            node->kind = ND_FOR;

            if(token->str != ';')
                 node->lhs = expr();

            consume(";");
            
            if(token->str != ';')
                 node->rhs = expr();

            consume(";");
            
            Node *C;

            if(token-> str != ';')
                C = expr();

            consume(";");

            if(!consume(")"))
                error_at(token->str," () is not found after 'if' ");

            Node *D;
            D = stmt();
      
            node = new_node(ND_FOR,node,D);
            node = new_node(ND_FOR, C, node);

            return node;
        }

        node = expr();
    }
    
    if(!consume(";"))
        error_at(token->str," ; is expected but doesn't exist.");
    return node;
    // Node *node =expr();
    // expect(";");
    // return node;
}

Node *expr(){

    #ifdef DEBUG_ON
        printf("expr() is called\n");
    #endif

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
        node = new_node(ND_ASSIGN,node,assign());

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

    //check the lval has already existed in locals;
    Token *tok = consume_ident();  // return lval token address (global token is moved forward)
    if(tok){ //if(tok != NULL)
        Node *node = calloc(1,sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if(lvar){ // tok is already existed in locals
            node->offset = lvar->offset;
        } else{ // register new tok in locals
            lvar = calloc(1,sizeof(LVar));
            lvar->next = locals; // pointer of current locals is next(before) lvals (previous lvals forward pointer)
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset =locals->offset + 8;
            node->offset = lvar->offset;
            locals=lvar; // pointer of current locals is updated with this lvar
        }

        return node;
        // node->offset = (tok->str[0] - 'a' + 1) * 8;
        // return node;
    }

    return new_node_num(expect_number());
}


