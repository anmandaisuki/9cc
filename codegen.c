#include "9cc.h"

void gen_lval(Node *node){
    if(node->kind != ND_LVAR)
        fprintf(stderr, "LVAL is not appropriate");

    printf("    mov rax, rbp\n"); // rbp is base address?
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

// recursive function : gen() is called in gen() 
void gen(Node *node){

    if(node->kind == ND_RETURN){
        gen(node->lhs); // for return value
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    }


    switch(node->kind){
        case ND_NUM:
            printf("    push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;

        case ND_IF: //if
            gen(node->lhs);
            printf("    pop rax \n");
            printf("    cmp rax, 0 \n");
            printf("    je  .LendXXX \n");
            gen(node->rhs);
            printf(".LendXXX: \n");
            return;

        case ND_IFELSE:
            if(node->lhs->kind != ND_IFELSE){
                gen(node->lhs);
                printf("    pop rax \n");
                printf("    cmp rax, 0 \n");
                printf("    je  .Lelse \n");
                gen(node->rhs);
                printf("    jmp .LendXXX \n");
                return;
            }
            gen(node->lhs);
            printf(".LelseXXX\n");
            gen(node->rhs);
            printf(".LendXXX\n");
            return;
        
        case ND_WHILE:
            printf(".LbeginXXX\n");
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .LendXXX\n");
            gen(node->rhs);
            printf("    jmp .LbeginXXX\n");
            printf(".LendXXX\n");
            return;
    }

    gen(node->lhs); // unless node->lhs is empty, gen() is called in gen(). The node without node->lhs is bottom of the tree
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) 
    {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");

    }

    printf("    push rax\n");

}


