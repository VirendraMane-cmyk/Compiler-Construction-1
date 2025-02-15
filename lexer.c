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

 int matchChar(char ch, char chToEqual) {
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
                if(matchChar(c,'<')){
                    state = 36;
                }
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
