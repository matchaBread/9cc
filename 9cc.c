#include "9cc.h"
#include <stdlib.h>
#include <stdio.h>

// Node *code[100];
Function *functions[100];
Variables *locals;
Variables *globals;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません．");
        return 1;
    }

    user_input = argv[1];

    tokenize();
    program();

    printf(".intel_syntax noprefix\n");

    for (Variables *vars = globals; vars; vars = vars->next)
    {
        printf(".comm   ");
        if (vars->scope != GLOBAL)
        {
            error("グローバル変数の読み込みに失敗しました．");
        }
        for (int i = 0; i < vars->len; i++)
        {
            printf("%c", vars->name[i]);
        }
        printf(", %d\n", vars->offset);
    }

    printf(".global ");

    for (int i = 0; functions[i]; i++)
    {
        for (int j = 0; j < functions[i]->length; j++)
        {
            printf("%c", functions[i]->name[j]);
        }
        if (functions[i + 1] != NULL)
        {
            printf(",");
        }
    }
    printf("\n\n");

    char *registers64[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    char *registers32[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
    char *registers8[6] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};

    for (int i = 0; functions[i]; i++)
    {
        for (int j = 0; j < functions[i]->length; j++)
        {
            printf("%c", functions[i]->name[j]);
        }

        int offset = 0;
        for (Variables *var = functions[i]->local_variables; var; var = var->next)
        {
            offset = var->offset;
        }
        printf(":\n");
        printf("    push rbp\n");
        printf("    mov rbp, rsp\n");
        printf("    sub rsp, %d\n", offset);

        int arg_index = 0;
        for (Variables *var = functions[i]->arguments; var; var = var->next)
        {
            if (arg_index > 5)
            {
                fprintf(stderr, "引数過多です．\n");
                exit(1);
            }

            printf("    mov rax, rbp\n");
            printf("    sub rax, %d\n", var->offset);
            switch (var->type->kind)
            {
            case POINTER:
                printf("    mov QWORD PTR [rax], %s\n", registers64[arg_index++]);
                break;
            case INT:
                printf("    mov DWORD PTR [rax], %s\n", registers32[arg_index++]);
                break;
            case CHAR:
                printf("    mov BYTE PTR [rax], %s\n", registers8[arg_index++]);
                break;
            default:
                error("引数に指定できない型です．");
            }
        }

        for (int j = 0; j < functions[i]->statements->size; j++)
        {
            Node *statement = get(functions[i]->statements, j);
            gen(statement);
            if (statement->kind != ND_RETURN)
            {
                printf("    pop rax\n");
            }
        }
    }

    return 0;
}