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
} TokenKind;

typedef struct Token Token; // "struct Token" -> "Token"

struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

Token *token; //global type. Used in several function or main()

void error(char *fmt, ...){
    va_list ap;     // when you use "..." as arguments, you have to use "va_list" or "va_start" in order to use "..."
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

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

//
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    token = token-> next;
    return true;
}

void expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op ){
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
Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token)); // reserve next token memory
    tok->kind = kind;
    tok->str = str; 
    cur->next = tok;
    return tok;
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

        if(*p == '+' || *p == '-'){

            cur = new_token(TK_RESERVED, cur, p++); // p++: incriment is done after procedure. ++p : incriment is done before procedure
            
            continue;
        }

        // check number or not
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10); //p is treated as string not char. So &p is regarded as string pointer. 10 is decimal
            // p is updated to next token. 
            // every input is char. 12 -> '1' '2' (ascii code) 0x51 0x52
            // strtol is translate ascii char to number
            continue;
        }

        // error(" tokenize failed");
        error_at(p,"tokenize process is failed. code is not correct. check the code indicated by ^ \n");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}


int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of arguments is not right ");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(argv[1]);


    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // initial char must be number
    printf("    mov rax, %d\n", expect_number()); 

    while(!at_eof()){ // unless end of file
        if(consume('+')){
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", expect_number());

    }

    printf("    ret\n");
    return 0;
}
