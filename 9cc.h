#ifndef _NINE_CC
#define _NINE_CC
#include <stdbool.h>

typedef enum
{
    TK_RESERVED, // reserved symbol
    TK_IDENT,    // identifier
    TK_NUM,      // integer token
    TK_EOF,      // EOF(End Of File) Token
    TK_RETURN,   // return statement
    TK_IF,       // if statement
    TK_ELSE,     // else statement
    TK_WHILE,    // while statement
    TK_FOR,      // while statement
} TokenKind;

typedef struct Token Token;

struct Token
{
    TokenKind kind; // kind(type) of token
    Token *next;    // next token pointer
    int val;        // evaluated value if kind == TK_NUM
    char *str;      // token string
    int len;        // token length
};

typedef enum
{
    ND_ADD,        // +
    ND_SUB,        // -
    ND_MUL,        // *
    ND_DIV,        // /
    ND_EQUAL,      // ==
    ND_NOT_EQUAL,  // !=
    ND_LESS_THAN,  // <
    ND_LESS_EQUAL, // <=
    ND_ASSIGN,     // =
    ND_LVAR,       // local variable
    ND_NUM,        // integer
    ND_RETURN,     // return
    ND_IF,         // if statement
    ND_WHILE,      // while statement
    ND_FOR,        // while statement
    ND_BLOCK,      // code block
    ND_CALL_FUNC,  // call function
} NodeKind;

typedef struct NodeReferenceVector NodeReferenceVector;

typedef struct Node Node;

typedef struct FunctionTable FunctionTable;

struct FunctionTable
{
    char *name;
    int length;
};

typedef struct FunctionTableLinkedList FunctionTableLinkedList;

struct FunctionTableLinkedList {
    FunctionTableLinkedList *next;
    FunctionTable *value;
};

struct Node
{
    int id;
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *other;
    Node *another;
    int val;                         // used if kind == ND_NUM
    int offset;                      // stack offset for variable(ND_LVAR)
    NodeReferenceVector *statements; // used if block code (ND_BLOCK)
    FunctionTable *function_table;             // used if ND_CALL_FUNC
};

struct NodeReferenceVector
{
    int size;
    int capacity;
    Node **elements;
    int element_size;
};

typedef struct LVar LVar;

struct LVar
{ // local variable list presented by linked list
    LVar *next;
    char *name;
    int len;
    int offset;
};

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();

void program();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

LVar *find_lvar(Token *tok);
FunctionTable *find_function(Token *tok);
void tokenize();
int is_alnum(char c);

void gen(Node *node);

NodeReferenceVector *new_vec();
void push(NodeReferenceVector *vector, Node *new_element);
Node *get(NodeReferenceVector *vector, int index);
/*
    global variables
 */
extern Token *token; // current token
extern char *user_input;
extern Node *code[100];
extern LVar *locals;
extern FunctionTableLinkedList *function_table;
extern int current_node_id;

#endif
