#include "lexer.h"
#include "interface.h"
#include "lexerDef.h"
#include "keyword_table.h"
#include <stdio.h>
// Toggle flag for buffers
KeywordTable* kt;
FILE* fp;
int num_of_rounds=0;
int line_no=0;
int state=0;
int retraction_flag=0;
twinBuffer* B;
#define MAX_LEXEME_LENGTH 21  // 20 characters + 1 for the null terminator

void initializeToken(tokenInfo *token) {
    // Set the token name to a default value.
    token->TOKEN_NAME = TK_UNKNOWN;

    // Allocate memory for the lexeme (20 characters max + 1 for '\0')
    token->LEXEME = (char*)malloc(MAX_LEXEME_LENGTH * sizeof(char));
    if (token->LEXEME == NULL) {
        fprintf(stderr, "Memory allocation error for token lexeme.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the allocated memory to 0
    memset(token->LEXEME, 0, MAX_LEXEME_LENGTH * sizeof(char));

    // Initialize other members to default values.
    token->LINE_NO = 0;
    token->IS_NUMBER = 0;
    token->VALUE = NULL;
}
void initializeBuffers(FILE* fp) {
    B = malloc(sizeof(twinBuffer));
    if (B == NULL) {
        perror("Failed to allocate twinBuffer");
        exit(EXIT_FAILURE);
    }
    
    B->buffer1 = malloc(BUFFER_SIZE * sizeof(char));
    B->buffer2 = malloc(BUFFER_SIZE * sizeof(char));
    if (B->buffer1 == NULL || B->buffer2 == NULL) {
        perror("Failed to allocate buffers");
        exit(EXIT_FAILURE);
    }
    state = 0;
    B->lexemeBegin = -1;
    B->forward = -1;
    B->lineNumber = 1;
    B->currentBuffer = 0;
    
    B->fp = fp; // or e.g., fopen("input.txt", "r")
}

void initializeLexer(FILE* f){
    fp = f;
    KeywordTable* kt=initializeTable();
    initializeBuffers(fp);
}
FILE* getStream()
{
    if (fp == NULL) {
        fprintf(stderr, "Error: File pointer is NULL.\n");
        return NULL;
    }
    
    B->fp = fp;
    B->currentBuffer = 0;  // We'll treat 0 as buffer1 and 1 as buffer2
    B->lineNumber = 1;
    
    size_t bytesRead = fread(B->buffer1, sizeof(char), BUFFER_SIZE, fp);
    
    if (bytesRead == 0) {
        B->buffer1[0] = '\0';  // Mark the buffer as empty
    } else {
        B->buffer1[bytesRead] = '\0'; // Null-terminate the buffer
    }
    
    B->forward = 0; // Initialize forward as an index
    
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

int stringToInteger(char* str) {
    int num;
    sscanf(str,"%d",&num);
    return num;
}

float stringToFloat(char* str) {
    float f;
    sscanf(str,"%f",&f);
    return f;
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

int getNextChar() {
    // Check if the buffer is uninitialized; use -1 as an initial flag
    if (B->lexemeBegin == -1 && B->forward == -1) {

        if (B->fp == NULL) {
            fprintf(stderr, "Error: twinBuffer file pointer is NULL.\n");
            return EOF;
        }
        // Initialize buffers for the first time
        if (getStream() == NULL) {
            return EOF;
        }
        B->lexemeBegin = 0;
        B->forward = 0;
        B->currentBuffer = 0;
    }
    
    // Determine the current buffer
    char* buffer = (B->currentBuffer == 0) ? B->buffer1 : B->buffer2;
    char curr_char = buffer[B->forward];
    
    // If we've reached the end of the current buffer, check for EOF
    if (curr_char == '\0') {
        return EOF;
    }
    
    // Check for buffer overflow. If we are at the last index, try to reload.
    if (B->forward >= BUFFER_SIZE - 1) {
        if (getStream(B->fp) == NULL || B->buffer1[0] == '\0') {
            return EOF;
        }
        // Toggle the current buffer (assuming you have logic to alternate buffers)
        B->currentBuffer = 1 - B->currentBuffer;
        B->forward = 0;
        buffer = (B->currentBuffer == 0) ? B->buffer1 : B->buffer2;
        curr_char = buffer[B->forward];
    } else {
        B->forward++;
    }
    
    // Only increment lineNumber if the character read is '\n'
    if (curr_char == '\n' && retraction_flag == 0) {
        B->lineNumber++;
    }
    
    // Unset the retraction flag if it was set
    if (retraction_flag == 1) {
        retraction_flag = 0;
    }
    //printf("%c\n",curr_char);
    return (int) curr_char;
}


tokenInfo getNextToken()
{
    
    //printf("hi\n");
    int tokenIndex = 0;
    int state = 0; // DFA initial state
    char c;
    int errorType = 0;

    // Clear the lexeme buffer.
    tokenInfo token;
    initializeToken(&token);
    //printf("here\n");
    // Skip whitespace and update line numbers.
    char* current_buffer = B->currentBuffer ? B->buffer1 : B->buffer2;
    while (isspace(current_buffer[B->forward]))
    {
        if (current_buffer[B->forward] == '\n')
            B->lineNumber++;
        B->forward++;
    }
    //printf("Hello!!!!!!!!\n");
    // DFA processing loop.
    while (1)
    {
        int temp = getNextChar();
        c = (char)(temp);
        printf("%c\n",c);
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
                c = getNextChar(B);
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
                c = getNextChar(B);
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
                c = getNextChar(B);
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
                c = getNextChar(B);
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
                c = getNextChar(B);
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
                c = getNextChar(B);
                if(c == '='){
                    state = 26;
                }
                else{
                    char *pattern = copyString(B->lexemeBegin,B->forward - sizeof(char));

                    printf("Line %d : Cannot recognize the pattern %s, Were you tring for == ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    //Report error types
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 26 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_EQ,lex,B->lineNumber,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 27 : {
                c = getNextChar(B);
                if(c == '@'){
                    state = 28;
                }
                else{
                    char *pattern = copyString(B->lexemeBegin,B->forward - sizeof(char));

                    printf("Line %d : Cannot recognize the pattern %s, Were you tring for @@@ ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    //Report error types
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 28 : {
                c = getNextChar(B);
                if(c == '@'){
                    state = 29;
                }
                else{
                    char *pattern = copyString(B->lexemeBegin,B->forward - sizeof(char));

                    printf("Line %d : Cannot recognize the pattern %s, Were you tring for @@@ ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    //Report error types
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 29 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_OR,lex,B->lineNumber,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 30 : {
                c = getNextChar(B);
                if(c == '&'){
                    state = 31;
                }
                else{
                    char *pattern = copyString(B->lexemeBegin,B->forward - sizeof(char));

                    printf("Line %d : Cannot recognize the pattern %s, Were you tring for &&& ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    //Report error types
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 31 : {
                c = getNextChar(B);
                if(c == '&'){
                    state = 29;
                }
                else{
                    char *pattern = copyString(B->lexemeBegin,B->forward - sizeof(char));

                    printf("Line %d : Cannot recognize the pattern %s, Were you tring for &&& ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    //Report error types
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 32 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                token = createToken(TK_AND,lex,B->lineNumber,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 33 : {
                c = getNextChar(B);
                while(c != '\n' && c != EOF){
                    c = getNextChar(B);
                }
                state = 34;
                break;
            }
            case 34 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                if(c == '\n'){
                    token = createToken(TK_COMMENT,lex,B->lineNumber - 1,0,NULL);
                }
                else{
                    token = createToken(TK_COMMENT,lex,B->lineNumber,0,NULL);
                }
                accept(B);
                return token;
                break;
            }
            case 35 : {
                c = getNextChar(B);
                if(checkInRange(c,'2','7')) {
                    state = 36;
                }
                else if(checkInRange(c,'a','z')) {
                    state = 40;
                }
                else {
                    state = 41;
                }
                break;
            }
            case 36 : {
                c = getNextChar(B);
                while(checkInRange(c,'b','d')){
                    c = getNextChar(B);
                }
                if(checkInRange(c,'2','7')){
                    state = 37;
                }
                else{
                    state = 39;
                }
                break;
            }
            case 37: {
                c = getNextChar(B);
                while(checkInRange(c,'2','7')){
                    c = getNextChar(B);
                }
                state = 38;
                break;
            }
            case 38 : {
                retract(1,B);
                int length = B->forward - B->lexemeBegin + 1;
                if(length < 2){
                    printf("Line No.: %d,Identifier length less than two not permitted 2\n",B->lineNumber);
                    errorType = 4;
                    state = 100;
                }
                else if(length > 20){
                    printf("Line No.: %d,Identifier length more than than not permitted 20\n",B->lineNumber);
                    errorType = 4;
                    state = 100;
                }
                else {
                    char* lex = copyString(B->lexemeBegin,B->forward);
                    if(c == '\n'){
                        token = createToken(TK_ID,lex,B->lineNumber-1,0,NULL);
                    }
                    else{
                        token = createToken(TK_ID,lex,B->lineNumber,0,NULL);
                    }
                    accept(B);
                    return token;
                }
                break;
            }
            case 39 : {
                retract(1,B);
                char* lex = copyString(B->lexemeBegin,B->forward);
                // Corner case => If c is newline character, then the token was one above the current linecount
                if(c == '\n'){
                    token = createToken(TK_ID,lex,B->lineNumber-1,0,NULL);
                }
                else{
                    token = createToken(TK_ID,lex,B->lineNumber,0,NULL);
                }
                accept(B);
                return token;
                break;
            }
            case 40 : {
                c = getNextChar(B);
                while(checkInRange(c,'a','z')){
                    c = getNextChar(B);
                }
                state = 41;
                break;
            }
            case 41 : {
                retract(1,B);
                char* lex = copyString(B->lexemeBegin,B->forward);
                NODE* n = lookUp(kt,lex);
                if(n == NULL) {
                    if(c == '\n')
                        token = createToken(TK_FIELDID,lex,B->lineNumber-1,0,NULL);
                    else
                        token = createToken(TK_FIELDID,lex,B->lineNumber,0,NULL);
                }
                else {
                    if(c == '\n')
                        token = createToken(n->TOKEN_NAME,lex,B->lineNumber-1,0,NULL);
                    else
                        token = createToken(n->TOKEN_NAME,lex,B->lineNumber,0,NULL);
                }
                accept(B);
                return token;
                break;
            }
            case 42: {
                c = getNextChar(B);
                while(checkInRange(c,'0','9'))
                    c = getNextChar(B);

                if(c == '.') {
                    c = getNextChar(B);
                    if(checkInRange(c,'0','9')){
                        retract(1,B);
                        state = 44;
                    }
                    else{
                        char* lex = copyString(B->lexemeBegin,B->forward);
                        token = createToken(TK_NUM,lex,B->lineNumber,0,NULL);
                        accept(B);
                        return token;
                    }
                }
                else {
                    state = 43;
                }
                break;
            }
            case 43 : {
                retract(1,B);
                char* lex = copyString(B->lexemeBegin,B->forward);
                Value* val = malloc(sizeof(Value));
                val->INT_VALUE = stringToInteger(lex);
                if(c == '\n')
                    token = createToken(TK_NUM,lex,B->lineNumber-1,1,val);
                else
                    token = createToken(TK_NUM,lex,B->lineNumber,1,val);
                accept(B);
                return token;
                break;
            }
            case 44 : {
                c = getNextChar(B);
                if(checkInRange(c,'0','9')) {
                    state = 45;
                }
                else {
                    //Goes to start state and will read the next inout stream char
                    //I hope
                    token = createToken(TK_DOT,'.',B->lineNumber,0,NULL);
                    retract(1,B);
                    accept(B);
                    return token;
                }
                break;
            }
            case 45 : {
                c = getNextChar(B);
                if(checkInRange(c,'0','9')) {
                    state = 46;
                }
                else {
                    // Throw lexical
                    char* pattern = copyString(B->lexemeBegin, B->forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for a real number ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 46 : {
                c = getNextChar(B);
                if(c == 'E'){
                    state = 55;
                }
                else{
                    retract(1,B);
                    char* lex = copyString(B->lexemeBegin,B->forward);
                    Value* val = (Value*)malloc(sizeof(Value));
                    val->FLOAT_VALUE = stringToFloat(lex);
                    token = createToken(TK_RNUM,lex,B->lineNumber,2,val);
                    accept(B);
                    return token;
                    break;
                }
            }
            case 47 : {
                c = getNextChar(B);
                if(checkInRange(c,'a','z') || checkInRange(c,'A','Z')) {
                    state = 48;
                }
                else {
                    // throw lexical error
                    char* pattern = copyString(B->lexemeBegin, B->forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for a function ID ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 48 : {
                c = getNextChar(B);
                while(checkInRange(c,'a','z') || checkInRange(c,'A','Z'))
                    c = getNextChar(B);

                if(checkInRange(c,'0','9')) {
                    state = 49;
                }
                else {
                    state = 51;
                }
                break;
            }
            case 49 : {
                c = getNextChar(B);
                while(checkInRange(c,'0','9'))
                    c = getNextChar(B);

                state = 50;
                break;
            }
            case 50 : {
                retract(1,B);
                int identifierLength = B->forward - B->lexemeBegin + 1;
                if(identifierLength > 30) {
                    printf("Line %d : Function identifier length exceeds 30 characters\n" ,B->lineNumber);
                    errorType = 4;
                    state = 100;
                }
                else {
                    char* lex = copyString(B->lexemeBegin,B->forward);
                    if(c == '\n')
                        token = createToken(TK_FUNID,lex,B->lineNumber,0,NULL);
                    else
                        token = createToken(TK_FUNID,lex,B->lineNumber-1,0,NULL);
                    accept(B);
                    return token;
                }
                break;
            }
            case 51 : {
                retract(1,B);
                char* lex = copyString(B->lexemeBegin,B->forward);
                NODE* n = lookUp(kt,lex);
                if(n == NULL) {
                    if(c == '\n')
                        token = createToken(TK_FUNID,lex,B->lineNumber-1,0,NULL);
                    else
                        token = createToken(TK_FUNID,lex,B->lineNumber,0,NULL);
                }
                else {
                    if(c == '\n')
                        token = createToken(n->TOKEN_NAME,lex,B->lineNumber-1,0,NULL);
                    else
                        token = createToken(n->TOKEN_NAME,lex,B->lineNumber,0,NULL);
                }
                accept(B);
                return token;
                break;
            }
            case 52 : {
                c = getNextChar(B);
                if(checkInRange(c,'a','z')) {
                    state = 53;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(B->lexemeBegin, B->forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for a record ID ?\n" ,B->lineNumber,pattern);
                    free(pattern);
                    errorType = 3;
                    state = 100;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1,B);
                }
                break;
            }
            case 53 : {
                c = getNextChar(B);
                while(checkInRange(c,'a','z'))
                    c = getNextChar(B);

                state = 54;
                break;
            }
            case 54: {
                retract(1,B);
                char* lex = copyString(B->lexemeBegin,B->forward);
                if(c == '\n')
                    token = createToken(TK_RECORDID,lex,B->lineNumber-1,0,NULL);
                else
                    token = createToken(TK_RECORDID,lex,B->lineNumber,0,NULL);
                accept(B);
                return token;
                break;
            }
            case 55 : {
                c = getNextChar(B);
                if(c == '+' || c == '-'){
                    state = 56;
                }
                else if(checkInRange(c,'0','9')){
                    state = 57;
                }
                else{
                    //Throw lexical error
                    char* pattern = copyString(B->lexemeBegin, B->forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s" ,B->lineNumber,pattern);
                    free(pattern);
                    errorType = 3;
                    state = 100;
                }
                break;
            }
            case 56 : {
                c = getNextChar(B);
                if(checkInRange(c,'0','9')){
                    state = 57;
                }
                else{
                    char* pattern = copyString(B->lexemeBegin, B->forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s" ,B->lineNumber,pattern);
                    free(pattern);
                    errorType = 3;
                    state = 100;
                }
                break;
            }
            case 58 : {
                c = getNextChar(B);
                if(checkInRange(c,'0','9')){
                    state = 59;
                }
                else{
                    char* pattern = copyString(B->lexemeBegin, B->forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s" ,B->lineNumber,pattern);
                    free(pattern);
                    errorType = 3;
                    state = 100;
                }
                break;
            }
            case 59 : {
                char* lex = copyString(B->lexemeBegin,B->forward);
                Value* val = (Value*)malloc(sizeof(Value));
                val->FLOAT_VALUE = stringToFloat(lex);
                token = createToken(TK_RNUM,lex,B->lineNumber,2,val);
                accept(B);
                return token;
                break;
            }
            case 100: {
                // Error State
                // Rationale 1 => Set a flag that error has reached for this input program so do not tokenize any further
                // Rationale 2 => Try to tokenize to the closest match and continue tokenizing further

                // Chosen Rationale => Panic mode, Travel up till a delimiter

                // Comment this, will bring it back to state 0
                // c = nextChar();
                // while(c != ';' && c !=  EOF && c != '\n') {
                //     c = nextChar();
                // }

                char* lex = copyString(B->lexemeBegin,B->forward);

                // A retraction only occurs if the errorType is 3, so check if the character read was a '\n'
                if(errorType == 3 && c == '\n')
                    token = createToken(TK_ERR,lex,B->lineNumber-1,errorType,NULL);
                else
                    token = createToken(TK_ERR,lex,B->lineNumber,errorType,NULL);
                accept(B);

                // Move back to the start state after setting lexemeBegin if an unidentifiable character causes the error
                return token;

                break;
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
void removeComments(char* testCaseFile, char* cleanFile) {
    // Open the test case file for reading.
    FILE* tcf = fopen(testCaseFile, "r");
    if (tcf == NULL) {
        printf("NOT ABLE TO OPEN FILE %s (pointer is NULL)\n", testCaseFile);
        return;
    }
    initializeBuffers(tcf);
    // Open the clean file for writing.
    FILE* out = fopen(cleanFile, "w");
    if (out == NULL) {
        printf("Error opening clean file %s\n", cleanFile);
        fclose(tcf);
        return;
    }
    //printf("HERE!!\n");
    
    getStream();
    
    // The check variable indicates the current state in comment removal.
    // 0: no comment detected; 2: inside a non-comment section; 3: inside a comment.
    int check = 0;
    int c;
    while ((c = getNextChar(&B)) != EOF) {
        char ch = (char)c; // Convert int to char for writing
        switch (check) {
            case 0:
                if (ch == ' ' || ch == '\f' || ch == '\r' || ch == '\t' || ch == '\v') {
                    fwrite(&ch, 1, 1, out);
                    check = 0;
                }
                else if (ch == '%') {
                    check = 3;
                }
                else if (ch == '\n') {
                    fwrite(&ch, 1, 1, out);
                    check = 0;
                }
                else {
                    fwrite(&ch, 1, 1, out);
                    check = 2;
                }
                break;
            case 2:
                fwrite(&ch, 1, 1, out);
                if (ch == '\n')
                    check = 0;
                break;
            case 3:
                if (ch == '\n') {
                    fwrite(&ch, 1, 1, out);
                    check = 0;
                }
                break;
        }
    }
    
    fclose(tcf);
    fclose(out);
}
