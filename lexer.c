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
tokenInfo getNextToken(twinBuffer *B)
{
    tokenInfo token;
    token.LINE_NO = B->lineNumber;
    int tokenIndex = 0;
    int state = 0; // DFA initial state
    char c;

    // Clear the lexeme buffer.
    memset(token.LEXEME, 0, sizeof(token.LEXEME));

    // Skip whitespace and update line numbers.
    while (isspace(*B->forward))
    {
        if (*B->forward == '\n')
            B->lineNumber++;
        B->forward++;
    }

    // DFA processing loop.
    while (1)
    {
        c = *B->forward;
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
                else if(checkSingleChar(c.'[')){
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
                else if(checkSingleChar(c,';')){
                    state=8;
                }
                else if(single)
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
