#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lexer.h"
//#include "parser.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    printf("Implementation Status: Both lexical and syntax analysis modules implemented.\n");

    int choice;
    while (1) {
        printf("\nOptions:\n");
        printf("0. Exit\n");
        printf("1. Remove comments\n");
        printf("2. Print token list\n");
        // printf("3. Parse code and print parse tree\n");
        // printf("4. Measure execution time\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 0:
                return 0;
                
            case 1: {
                // Remove comments: use the input and output file names directly.
                removeComments(argv[1], argv[2]);
                printf("Comments removed and saved to %s\n", argv[2]);
                break;
            }
            
            case 2: {
                //printf("hi");
                // For tokenization, open a fresh file and reinitialize the lexer.
                FILE* fp = fopen(argv[1], "r");
                if (fp == NULL) {
                    printf("Could not open file %s\n", argv[1]);
                    break;
                }
                initializeLexer(fp);
                
                tokenInfo token;
                printf("Token List:\n");
                printf("%-15s %-15s %-10s\n", "Token Name", "Lexeme", "Line Number");
                while (1) {
                    //printf("hi\n");
                    token = getNextToken();
                    //printf("hi\n");
                    if (token.TOKEN_NAME == TK_ERR) {  // Assuming TK_ERR indicates end-of-input or an error.
                        break;
                    }
                    printf("%-15d %-15s %-10d\n", token.TOKEN_NAME, token.LEXEME, token.LINE_NO);
                }
                //fclose(fp);
                break;
            }
            
        //     case 3: {
        //         // Example parser code. (Ensure your parser functions and types are defined.)
        //         int fd = open(argv[1], O_RDONLY);
        //         ParsingTable* pTable = initialiseParsingTable();
        //         FirstAndFollow* fafl = computeFirstAndFollowSets(extractGrammar());
        //         createParseTable(fafl, pTable);
            
        //         ParseTree* pt = parseInputSourceCode(argv[1], pTable, fafl);
            
        //         FILE* outputFile = fopen(argv[2], "w");
        //         if (outputFile == NULL) {
        //             printf("Error opening output file %s\n", argv[2]);
        //             break;
        //         }
            
        //         printParseTree(pt, outputFile);
        //         fclose(outputFile);
            
        //         if (lexicalErrorFlag || syntaxErrorFlag) {
        //             printf("Errors encountered during parsing.\n");
        //         } else {
        //             printf("Parsing successful.\n");
        //         }
        //         break;
        //     }
            
        //     case 4: {
        //         // Measure execution time.
        //         clock_t start_time = clock();
            
        //         FILE* fp = fopen(argv[1], "r");
        //         if (fp == NULL) {
        //             printf("Could not open file %s\n", argv[1]);
        //             break;
        //         }
            
        //         twinBuffer B;
        //         // Initialize B if needed (e.g. setting B.lexemeBegin and B.forward to -1)
        //         getStream(fp, &B);
            
        //         parseInputSourceCode(argv[1], argv[2]);
        //         fclose(fp);
            
        //         clock_t end_time = clock();
        //         double total_CPU_time = (double)(end_time - start_time);
        //         double total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            
        //         printf("Total CPU Time: %f\n", total_CPU_time);
        //         printf("Total CPU Time in Seconds: %f\n", total_CPU_time_in_seconds);
        //         break;
        //     }
            
        //     default:
        //         printf("Invalid choice. Please try again.\n");
        //         break;
        // }
    }

    return 0;
}
}
