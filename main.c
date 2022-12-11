#include "9cc.h"

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of arguments is not right ");
        return 1; 
    }

    user_input = argv[1];
    token = tokenize(argv[1]);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

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

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
