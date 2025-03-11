#ifndef KEYWORD_TABLE
#define KEYWORD_TABLE
#include "keyword_tableDef.h"

// Functions for keyword hashtable
int hashFunction(char* str);
void addEntry(KeywordTable* kt,TokenName tn, char* lexeme);
Node* lookUp(KeywordTable* kt,char* lexeme);
KeywordTable* initializeTable();
void printHashTable(KeywordTable* kt);
// Functions for List
Node* addToList(Node* ls, TokenName tn, char* lexeme);
int searchList(Node* ls, char* lexeme);
int printList(Node* ls);

#endif //KEYWORD