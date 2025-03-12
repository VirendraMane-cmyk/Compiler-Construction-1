/* Group 13
Virendrasinh Mane                           2022A7PS1175P
Vishnu Chebolu                              2022A7PS0124P
Shashwat Goyal                              2022A7PS0115P
Parthib Sarkar                              2022A7PS0111P
Samrath Singh Khanuja                       2022A7PS1171P
Harsimar Singh Saluja                       2022A7PS1187P*/

#ifndef LEX_DEF_
#define LEX_DEF_

#define BUFFER_SIZE 4096  // Size of each buffer
#define EOF_MARKER '\0'   // End of buffer marker
#include<stdio.h>

typedef enum TokenName {
    TK_ASSIGNOP,
    TK_COMMENT,
    TK_ID,
    TK_RUID,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_NUM,
    TK_RNUM,
    TK_FIELDID,
    TK_FUNID,
    TK_RECORDID,
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_SQL,
    TK_SQR,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_ENDWHILE,
    TK_OP,
    TK_CL,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE,
    TK_ERR
} TokenName;

typedef union Value {
    int INT_VALUE;
    float FLOAT_VALUE;
} Value;

typedef struct Token {
    TokenName TOKEN_NAME;
    char* LEXEME;
    int LINE_NO;

    // Lexical Error Codes:
    // 0: Not a number
    // 1: Integer
    // 2: Real Number
    // 3: Token identification fails
    // 4: Token identified, but constraints violated
    // 5: Two identifiers declared back-to-back
    // 6: Unknown symbol (starting point)
    // 7: Invalid character in identifier
    // 8: Identifier exceeds maximum length
    // 9: Unterminated string literal
    // 10: Invalid escape sequence in string literal
    // 11: Numeric constant exceeds maximum value
    // 12: Ill-formed floating-point constant (e.g., multiple decimal points)
    // 13: Invalid exponent format in floating-point number
    // 14: Missing digits after decimal point
    // 15: Symbol sequence is close to a valid token but incorrect (e.g., "<-" instead of "<--")
    // 16: Invalid Unicode character
    // 17: Unexpected end-of-file (EOF) within a literal or comment
    // 18: Zero length identifier
    // 19: Invalid character at start of number (e.g. `a123`)
    // 20: End of line in string literal
    // 21: Comment not closed

    int IS_NUMBER;

    Value* VALUE; // Stores NULL if the Token is not a number
} tokenInfo;

typedef struct twinBuffer {  
    char* buffer1;  // First half of the buffer  
    char* buffer2;  // Second half of the buffer  
    int lexemeBegin;          // Pointer to the beginning of the lexeme  
    int forward;  
    int lineNumber;  
    int currentBuffer;
    FILE* fp;          // Pointer to the current character being analyzed  
} twinBuffer;  

typedef struct NODE{
    TokenName TOKEN_NAME;
    char* LEXEME;
    struct NODE* next;
}NODE;

typedef struct KEYWORDNODE{
    NODE* keyWords;
}KEYWORDNODE;

typedef struct KEYWORDTABLE{
    KEYWORDNODE* KEYWORDS;
}KEYWORDTABLE;

extern int line_no;
extern int num_of_rounds;
extern int state;
extern int retraction_flag;

#endif
