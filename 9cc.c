#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of arguments is not right ");
        return 1;
    }

    char *p = argv[1]; // p is treated as string. string doesnt exist in C


    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("    mov rax, %ld\n", strtol(p, &p,10)); //p is treated as string not char. So &p is regarded as string pointer. 10 is decimal

    while(*p){
        if(*p == '+'){
            p++;
            printf("    add rax, %ld\n",strtol(p,&p,10));
            continue;
        }

        if(*p == '-'){
            p++;
            printf("    sub rax, %ld\n",strtol(p,&p,10));
            continue;
        }

        fprintf(stderr, "not expected string: '%c'\n", *p); // write error to stderr(file pointer)
        return 1;
    }

    printf("    ret\n");
    return 0;
}
