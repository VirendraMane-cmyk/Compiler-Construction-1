#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"

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
        printf("3. Parse code and print parse tree\n");
        printf("4. Measure execution time\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                return 0;
            case 1:
                char cleanFile[256];
                sprintf(cleanFile, "%s_no_comments.txt", argv[1]);
                removeComments(argv[1], cleanFile);
                printf("Comments removed and saved to %s\n", cleanFile);
                break;
            case 2:
                FILE* fp = fopen(argv[1], "r");
                if (fp == NULL) {
                    printf("Could not open file %s\n", argv[1]);
                    break;
                }
            
                twinBuffer B;
                getStream(fp);
            
                tokenInfo token;
                printf("Token List:\n");
                printf("%-15s %-15s %-10s\n", "Token Name", "Lexeme", "Line Number");
                while (1) {
                    token = getNextToken(&B);
                    if (token.TOKEN_NAME == EOF_TOKEN) {
                        break;
                    }
                    printf("%-15d %-15s %-10d\n", token.TOKEN_NAME, token.LEXEME, token.lineNumber);
                }
                fclose(fp);
                break;
            
            case 3:
                // Initialize the parser and lexer
                int fd = open(argv[1], O_RDONLY);
                ParsingTable* pTable = initialiseParsingTable();
                FirstAndFollow* fafl = computeFirstAndFollowSets(extractGrammar());
                createParseTable(fafl, pTable);
            
                // Parse the input source code
                ParseTree* pt = parseInputSourceCode(argv[1], pTable, fafl);
            
                // Print the parse tree to the specified output file
                FILE* outputFile = fopen(argv[2], "w");
                if (outputFile == NULL) {
                    printf("Error opening output file %s\n", argv[2]);
                    break;
                }
            
                // Assuming you have a function to print the parse tree
                printParseTree(pt, outputFile);
            
                fclose(outputFile);
            
                // Check for errors
                if (lexicalErrorFlag || syntaxErrorFlag) {
                    printf("Errors encountered during parsing.\n");
                } else {
                    printf("Parsing successful.\n");
                }
            
                break;
            
                break;
            case 4:
                clock_t start_time = clock();
            
                // Call the parser function here (assuming it's named parseInputSourceCode)
                // This will invoke both lexer and parser.
                FILE* fp = fopen(argv[1], "r");
                if (fp == NULL) {
                    printf("Could not open file %s\n", argv[1]);
                    break;
                }
            
                twinBuffer B;
                getStream(fp);
            
                // Assuming you have a parser function named parseInputSourceCode
                // that takes the input file and output file names as arguments.
                // Adjust the function name and parameters according to your parser implementation.
                parseInputSourceCode(argv[1], argv[2]);
            
                fclose(fp);
            
                clock_t end_time = clock();
            
                double total_CPU_time = (double)(end_time - start_time);
                double total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            
                printf("Total CPU Time: %f\n", total_CPU_time);
                printf("Total CPU Time in Seconds: %f\n", total_CPU_time_in_seconds);
            
                break;
            
            default:
                printf("Invalid choice. Please try again.\n");
                continue;
        }
    }

    return 0;
}

