#include "9cc.h"

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of arguments is not right ");
        return 1; 
    }

    user_input = argv[1];
    token = tokenize(argv[1]);
    
#ifdef DEBUG_ON
//For Debug begin 
    // TOken checkment
    Token *token_shadow = token;
    for(int i = 1 ; i < 10; i++){

        printf("No%d token_kind[%d], token_len[%d], token_str_initial[%c] \n",i,token_shadow->kind,token_shadow->len,*token_shadow->str);
        token_shadow = token_shadow->next;
    }
    printf("\n");
//For Debug end
#endif

    locals = calloc(1,sizeof(LVar));
    // Node *node = expr();
    program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    // // initial char must be number
    // printf("    mov rax, %d\n", expect_number()); 

    // while(!at_eof()){ // unless end of file
    //     if(consume('+')){
    //         printf("    add rax, %d\n", expect_number());
    //         continue;
    //     }
    //     expect('-');
    //     printf("    sub rax, %d\n", expect_number());
    // }

    for(int i = 0; code[i]; i++){
        gen(code[i]);
        printf("    pop rax\n");
    }

    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}
