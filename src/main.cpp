#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "translator.h"

// таблиця лексем
Token* TokenTable;
// кількість лексем
unsigned int TokensNum;

// таблиця ідентифікаторів
Id* IdTable;
// кількість ідентифікаторів
unsigned int IdNum;

// Function to validate file extension
int hasValidExtension(const char* fileName, const char* extension)
{
    const char* dot = strrchr(fileName, '.');
    if (!dot || dot == fileName) return 0; // No extension found
    return strcmp(dot, extension) == 0;
}

int main(int argc, char* argv[])
{
    // виділення пам'яті під таблицю лексем
    TokenTable = new Token[MAX_TOKENS];

    // виділення пам'яті під таблицю ідентифікаторів
    IdTable = new Id[MAX_IDENTIFIER];

    char InputFile[32] = "";

    FILE* InFile;

    if (argc != 2)
    {
        printf("Input file name: ");
        gets_s(InputFile);
    }
    else
    {
        strcpy_s(InputFile, argv[1]);
    }

    // Check if the input file has the correct extension
    if (!hasValidExtension(InputFile, ".k09"))
    {
        printf("Error: Input file has invalid extension.\n");
        return 1;
    }

    if ((fopen_s(&InFile, InputFile, "rt")) != 0)
    {
        printf("Error: Cannot open file: %s\n", InputFile);
        return 1;
    }

    char NameFile[32] = "";
    int i = 0;
    while (InputFile[i] != '.' && InputFile[i] != '\0')
    {
        NameFile[i] = InputFile[i];
        i++;
    }
    NameFile[i] = '\0';

    char TokenFile[32];
    strcpy_s(TokenFile, NameFile);
    strcat_s(TokenFile, ".token");

    char ErrFile[32];
    strcpy_s(ErrFile, NameFile);
    strcat_s(ErrFile, "_errors.txt");

    FILE* errFile;
    if (fopen_s(&errFile, ErrFile, "w") != 0)
    {
        printf("Error: Cannot open file for writing: %s\n", ErrFile);
        return 1;
    }

    TokensNum = GetTokens(InFile, TokenTable, errFile);

    PrintTokensToFile(TokenFile, TokenTable, TokensNum);
    fclose(InFile);

    printf("\nLexical analysis completed: %d tokens. List of tokens in the file %s\n", TokensNum, TokenFile);
    printf("\nList of errors in the file %s\n", ErrFile);

    Parser(errFile);
    fclose(errFile);
    //ASTNode* ASTree = ParserAST();

    //char AST[32];
    //strcpy_s(AST, NameFile);
    //strcat_s(AST, ".ast");
    //// Open output file
    //FILE* ASTFile;
    //fopen_s(&ASTFile, AST, "w");
    //if (!ASTFile)
    //{
    //    printf("Failed to open output file.\n");
    //    exit(1);
    //}
    //PrintASTToFile(ASTree, 0, ASTFile);
    //printf("\nAST has been created and written to %s.\n", AST);

    char OutputFile[32];
    strcpy_s(OutputFile, NameFile);
    strcat_s(OutputFile, ".c");

    FILE* outFile;
    fopen_s(&outFile, OutputFile, "w");
    if (!outFile)
    {
        printf("Failed to open output file.\n");
        exit(1);
    }
    // генерація вихідного С коду
    generateCCode(outFile);
    printf("\nC code has been generated and written to %s.\n", OutputFile);

    fclose(outFile);

    fopen_s(&outFile, OutputFile, "r");
    char ExecutableFile[32];
    strcpy_s(ExecutableFile, NameFile);
    strcat_s(ExecutableFile, ".exe");
    compile_to_exe(OutputFile, ExecutableFile);

    /*char OutputFileFromAST[32];
    strcpy_s(OutputFileFromAST, NameFile);
    strcat_s(OutputFileFromAST, "_fromAST.c");

    FILE* outFileFromAST;
    fopen_s(&outFileFromAST, OutputFileFromAST, "w");
    if (!outFileFromAST)
    {
        printf("Failed to open output file.\n");
        exit(1);
    }
    generateCodefromAST(ASTree, outFileFromAST);
    printf("\nC code has been generated and written to %s.\n", OutputFileFromAST);

    fclose(outFileFromAST);

    fopen_s(&outFileFromAST, OutputFileFromAST, "r");
    char ExecutableFileFromAST[32];
    strcpy_s(ExecutableFileFromAST, NameFile);
    strcat_s(ExecutableFileFromAST, "_fromAST.exe");
    compile_to_exe(OutputFileFromAST, ExecutableFileFromAST);*/

    // Close the file
    _fcloseall();

    /*destroyTree(ASTree);*/

    delete[] TokenTable;
    delete[] IdTable;

    return 0;
}
