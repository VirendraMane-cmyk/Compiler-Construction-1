#include "lexer.h"
#include "interface.h"
#include "lexerDef.h"
    // Toggle flag for buffers

FILE *getStream(FILE *fp) {
    if (!fp) return NULL;  // If file pointer is NULL, return NULL

    size_t bytesRead;
    char *currentBuffer = useBuffer1 ? buffer1 : buffer2;  // Select buffer

    bytesRead = fread(currentBuffer, sizeof(char), BUFFER_SIZE, fp);
    if (bytesRead == 0) {
        return NULL;  // No more data left to read
    }

    currentBuffer[bytesRead] = EOF_MARKER;  // Mark the end of valid data

    lexemeBegin = forward = currentBuffer; // Reset pointers to buffer start
    useBuffer1 = !useBuffer1;  // Toggle buffer for next read

    return fp;  // Return file pointer for continued processing
}
tokenInfo getNextToken(twinBuffer B){
    char c;
    tokenInfo token;
    while(1){
        token.LINE_NO = line_no;
        switch(state)
        {
            case 0:;
        }
    }

}
void removeComments(char *testcaseFile, char *cleanFile) {
    FILE *fpIn = fopen(testcaseFile, "r");
    if (!fpIn) {
        perror("Error opening test case file");
        return;
    }
    
    FILE *fpOut = fopen(cleanFile, "w");
    if (!fpOut) {
        perror("Error opening clean file");
        fclose(fpIn);
        return;
    }
    
    int c;
    while ((c = fgetc(fpIn)) != EOF) {
        // Check for string literal start
        if (c == '\"') {
            fputc(c, fpOut);
            while ((c = fgetc(fpIn)) != EOF) {
                fputc(c, fpOut);
                if (c == '\\') { // Handle escape sequence inside string
                    c = fgetc(fpIn);
                    if (c == EOF)
                        break;
                    fputc(c, fpOut);
                } else if (c == '\"') {
                    break; // End of string literal
                }
            }
        }
        // Check for character literal start
        else if (c == '\'') {
            fputc(c, fpOut);
            while ((c = fgetc(fpIn)) != EOF) {
                fputc(c, fpOut);
                if (c == '\\') { // Handle escape sequence inside character literal
                    c = fgetc(fpIn);
                    if (c == EOF)
                        break;
                    fputc(c, fpOut);
                } else if (c == '\'') {
                    break; // End of character literal
                }
            }
        }
        // Check for potential comment start
        else if (c == '/') {
            int next = fgetc(fpIn);
            // Single-line comment detected
            if (next == '/') {
                // Skip characters until end-of-line or EOF
                while ((c = fgetc(fpIn)) != EOF && c != '\n');
                if (c == '\n') {
                    fputc(c, fpOut);  // Preserve newline to maintain line numbers
                }
            }
            // Multi-line comment detected
            else if (next == '*') {
                int prev = 0;
                // Skip until the closing "*/" is found
                while ((c = fgetc(fpIn)) != EOF) {
                    if (prev == '*' && c == '/') {
                        break;
                    }
                    prev = c;
                }
            }
            // Not a comment – just a '/' character
            else {
                fputc(c, fpOut);
                if (next != EOF)
                    fputc(next, fpOut);
            }
        }
        // Normal character; write it to output
        else {
            fputc(c, fpOut);
        }
    }
    
    fclose(fpIn);
    fclose(fpOut);
}
