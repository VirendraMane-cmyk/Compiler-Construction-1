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
tokenInfo getNextToken(twinBuffer *B)
{
    tokenInfo token;
    token.lineNumber = B->lineNumber;
    int tokenIndex = 0;
    int state = 0; // DFA initial state
    char c;

    // Clear the lexeme buffer.
    memset(token.lexeme, 0, sizeof(token.lexeme));

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
            if (isalpha(c))
            {
                token.lexeme[tokenIndex++] = c;
                state = 1; // Transition to identifier state.
                B->forward++;
            }
            else if (isdigit(c))
            {
                token.lexeme[tokenIndex++] = c;
                state = 2; // Transition to number state.
                B->forward++;
            }
            else if (c == '+')
            {
                token.lexeme[tokenIndex++] = c;
                token.lexeme[tokenIndex] = '\0';
                token.tokenType = TOKEN_OPERATOR;
                B->forward++;
                return token;
            }
            else if (c == '-')
            {
                token.lexeme[tokenIndex++] = c;
                token.lexeme[tokenIndex] = '\0';
                token.tokenType = TOKEN_OPERATOR;
                B->forward++;
                return token;
            }
            else if (c == '\0')
            { // End-of-file or end-of-buffer.
                token.tokenType = TOKEN_EOF;
                strcpy(token.lexeme, "EOF");
                return token;
            }
            else
            {
                // For any other character, simply return it as an operator for now.
                token.lexeme[tokenIndex++] = c;
                token.lexeme[tokenIndex] = '\0';
                token.tokenType = TOKEN_OPERATOR;
                B->forward++;
                return token;
            }
            break;

        case 1: // Identifier state.
            if (isalnum(c))
            {
                token.lexeme[tokenIndex++] = c;
                B->forward++;
            }
            else
            {
                token.lexeme[tokenIndex] = '\0';
                token.tokenType = TOKEN_IDENTIFIER;
                return token;
            }
            break;

        case 2: // Number state.
            if (isdigit(c))
            {
                token.lexeme[tokenIndex++] = c;
                B->forward++;
            }
            else
            {
                token.lexeme[tokenIndex] = '\0';
                token.tokenType = TOKEN_NUMBER;
                return token;
            }
            break;

        default:
            token.tokenType = TOKEN_ERROR;
            strcpy(token.lexeme, "Unknown token");
            return token;
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
