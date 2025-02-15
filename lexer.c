#include "lexer.h"
#include "interface.h"
#include "lexerDef.h"
// Toggle flag for buffers

FILE *getStream(FILE *fp)
{
    if (fp == NULL)
    {
        fprintf(stderr, "Error: File pointer is NULL.\n");
        return NULL;
    }
    TB.fp = fp;
    TB.currentBuffer = 1;
    TB.lineNumber = 1;
    size_t bytesRead = fread(TB.buffer1, sizeof(char), BUFFER_SIZE, fp);
    TB.buffer1[bytesRead] = '\0'; // Null-terminate the buffer
    TB.forward = TB.buffer1;
    return fp;
}

/*
 * getNextToken
 *
 * Implements a DFA-based lexer using a while loop and switch statement.
 * It processes the input character-by-character to form tokens.
 *
 * The DFA uses:
 *  - state 0 as the initial state,
 *  - state 1 for processing identifiers,
 *  - state 2 for processing numeric literals.
 *
 * This is a rough implementation; error handling, buffer switching, and
 * additional token types can be added as needed.
 */

 void retract(int amt,twinBuffer* B) {
    while(amt > 0) {
        --(B->forward);
        --amt;
    }
    
    retraction_flag = 1;
}

int checkInRange(char ch,char start, char end) {
    if(ch >= start && ch <= end)
        return 1;
    return 0;
}
int checkSingleChar(char ch, char chToEqual) {
    if(ch == chToEqual)
        return 1;
    return 0;
}

char* copyString(char* start, char* end) {
    char* store = (char*)malloc((end-start+2)*sizeof(char));
    char* trav = start;
    int i = 0;
    // Note <= was erroneous , corrected to <
    while(trav < end) {
        store[i] = *trav;
        ++trav;
        ++i;
    }

    // DOUBT - As per the instructor, the lexeme should not include the end marker
    store[i] = '\0';
    return store;
}

tokenInfo createToken(TokenName tokenName,char* lexeme,int lineNumber,int isNumber,Value* value){
    tokenInfo token;
    token.TOKEN_NAME = tokenName;
    token.IS_NUMBER = isNumber;
    token.LEXEME = lexeme;
    token.VALUE = value;
    return token;
}

void accept(twinBuffer* B){
    B->lexemeBegin = B->forward;
    return;
}

tokenInfo getNextToken(twinBuffer *B)
{
    int tokenIndex = 0;
    int state = 0; // DFA initial state
    char c;
    int errorType = 0;

    // Clear the lexeme buffer.
    tokenInfo token;
    memset(token.LEXEME, 0, sizeof(token.LEXEME));

    // Skip whitespace and update line numbers.
    while (isspace(B->forward))
    {
        if (B->forward == '\n')
            B->lineNumber++;
        B->forward++;
    }

    // DFA processing loop.
    while (1)
    {
        c = B->forward;
        switch (state)
        {
            case 0: // Start state
            if(isalpha(c)){
                if(checkSingleChar(c,'<')){
                    state = 36;
                }
                else if(checkSingleChar(c,'#')){
                    state = 52;
                }
                else if(checkInRange(c,'b','d')){
                    state = 35;
                }
                else if(checkSingleChar(c,'a') || checkInRange(c,'e','z')){
                    state=40;
                }
                else if(checkSingleChar(c,'_')){
                    state=47;
                }
                else if(checkSingleChar(c,'[')){
                    state=12;
                }
                else if(checkSingleChar(c,']')){
                    state=13;
                }
                else if(checkSingleChar(c,',')){
                    state=11;
                }
                else if(checkSingleChar(c,';')){
                    state=8;
                }
                else if(checkSingleChar(c,':')){
                    state=9;
                }
                else if(checkSingleChar(c,'.')){
                    state=10;
                }
                else if(checkSingleChar(c,')')){
                    state=6;
                }
                else if(checkSingleChar(c,'(')){
                    state=5;
                }
                else if(checkSingleChar(c,'+')){
                    state=1;
                }
                else if(checkSingleChar(c,'-')){
                    state=2;
                }
                else if(checkSingleChar(c,'*')){
                    state=3;
                }
                else if(checkSingleChar(c,'/')){
                    state=4;
                }
                else if(checkSingleChar(c,'~')){
                    state=7;
                }
                else if(checkSingleChar(c,'!')){
                    state=14;
                }
                else if(checkSingleChar(c,'>')){
                    state=22;
                }
                else if(checkSingleChar(c,'=')){
                    state=25;
                }
                else if(checkSingleChar(c,'@')){
                    state=27;
                }
                else if(checkSingleChar(c,'&')){
                    state=30;
                }
                else if(checkSingleChar(c,'%')){
                    state=33;
                }
                else if(checkInRange(c,'0','9')){
                    state=42;
                }
                else if(checkSingleChar(c,' ') || checkSingleChar(c, '\f') || checkSingleChar(c,'\t') || checkSingleChar(c,'\v')) {
                    B->lexemeBegin++;
                    state=0;
                }
                else if(checkSingleChar(c,'\n')) {
                    B->lexemeBegin++;
                    state=0;
                }
                else if(checkSingleChar(c,EOF)) {
                    return token;
                }
                else {
                    printf("Line %d : Invalid Character -> %c \n", B->lineNumber,c);
                    errorType = 6;
                    state = 100;
                    //Random Undefined State dala hai abhi ke liye - Shashwat
                }
                break;
            }
            case 1 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_PLUS,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 2 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_MINUS,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 3 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_MUL,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 4 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_DIV,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 5 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_OP,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 6 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_CL,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 7 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_NOT,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 8 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_SEM,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 9 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_COLON,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 10 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_DOT,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 11 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_COMMA,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 12 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_SQL,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 13 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_SQR,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 14 : {
                c = B->forward;
                if(c == '='){
                    state = 15;
                }
                else {
                    //Error found here -> Expected != found only ! 
                    char *pattern = copyString(B->lexemeBegin,B->forward - sizeof(char));
                    printf("Line %d : Not able to recognize %s, Were you trying for != ?\n",B->lineNumber,pattern);
                    //Free the memory allocated for the pattern printing
                    free(pattern);
                    errorType = 3;
                    state = 100;

                    retract(1,B);
                }
                break;
            }
            case 15 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_NE,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 16 : {
                c = B->forward;
                if(c == '='){
                    state = 20;
                }
                else if(c == '-'){
                    state = 17;
                }
                else {
                    state = 21;
                }
                break;
            }
            case 17 : {
                c = B->forward;
                if(c == '-'){
                    state = 18;
                }
                else{
                    //Double retract and recognize the symbols < and - seperately 
                    retract(1, B);
                    /* Now produce the token for '<'. We assume TK_LT is the token for '<'. */
                    char* lex = copyString(B->lexemeBegin, B->lexemeBegin + 1);
                    token = createToken(TK_LT, lex, B->lineNumber, 0, NULL);
                    /* Advance the pointer past the '<' and update lexemeBegin */
                    B->forward = B->lexemeBegin + 1;
                    accept(B);
                    return token;
                }
                break;
            }
            case 18 : {
                c = B->forward;
                if(c == '-'){
                    state = 19;
                }
                else{
                    //Double retract
                    //HOPE THIS WORKS
                    //PLEASE WORK 
                    retract(1,B);
                    retract(1,B);
                    char* lex = copyString(B->lexemeBegin,B->lexemeBegin + 1);
                    token = createToken(TK_LT, lex, B->lineNumber, 0, NULL);
                    B->forward = B->lexemeBegin + 1;
                    accept(B);

                    return token;
                }
                break;
            }
            case 19 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_ASSIGNOP,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 20 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_LE,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 21 : {
                retract(1,B);
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_LT,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 22 : {
                c = B->forward;
                if(c == '=') {
                    state = 23;
                }
                else {
                    state = 24;
                }
                break;
            }
            case 23 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_GE,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 24 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_GT,lex,line_no,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 25 : {
                c = B->forward;
            }


        }
    }
}

/*
 * removeComments
 *
 * Reads the source file (testcaseFile) and writes a "clean" file (cleanFile)
 * with comments removed. Handles both single-line (//) and multi-line (/* ... *\/)
 * comments.
 *
 * This function is intended to be invoked once (e.g., via a driver) for evaluation.
 */
void removeComments(char *testcaseFile, char *cleanFile)
{
    FILE *src = fopen(testcaseFile, "r");
    if (!src)
    {
        fprintf(stderr, "Error opening source file: %s\n", testcaseFile);
        return;
    }
    FILE *dest = fopen(cleanFile, "w");
    if (!dest)
    {
        fprintf(stderr, "Error opening destination file: %s\n", cleanFile);
        fclose(src);
        return;
    }
    int c, next;
    while ((c = fgetc(src)) != EOF)
    {
        if (c == '/')
        {
            next = fgetc(src);
            if (next == EOF)
            {
                fputc(c, dest);
                break;
            }
            // Single-line comment: skip until newline.
            if (next == '/')
            {
                while ((c = fgetc(src)) != EOF && c != '\n')
                    ;
                if (c != EOF)
                    fputc(c, dest); // Preserve newline.
            }
            // Multi-line comment: skip until closing "*/" is found.
            else if (next == '*')
            {
                int prev = 0;
                while ((c = fgetc(src)) != EOF)
                {
                    if (prev == '*' && c == '/')
                        break;
                    prev = c;
                }
            }
            else
            {
                // Not a comment: write both characters.
                fputc(c, dest);
                fputc(next, dest);
            }
        }
        else
        {
            // Write normal characters.
            fputc(c, dest);
        }
    }
    fclose(src);
    fclose(dest);
}
