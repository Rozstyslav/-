#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "translator.h"
#include <iostream>
#include <string>

// таблиц¤ лексем
extern Token* TokenTable;
// к≥льк≥сть лексем
extern unsigned int TokensNum;

// таблиц¤ ≥дентиф≥катор≥в
extern Id* IdTable;
// к≥льк≥сть ≥дентиф≥катор≥в
extern unsigned int IdNum;

static int pos = 0;

// наб≥р функц≥й дл¤ рекурсивного спуску 
// на кожне правило - окрема функц≥¤
void program(FILE* errFile);
void variable_declaration(FILE* errFile);
void variable_list(FILE* errFile);
void program_body(FILE* errFile);
void statement(FILE* errFile);
void assignment(FILE* errFile);
void arithmetic_expression(FILE* errFile);
void term(FILE* errFile);
void factor(FILE* errFile);
void input(FILE* errFile);
void output(FILE* errFile);
void conditional(FILE* errFile);

void goto_statement(FILE* errFile);
void label_statement(FILE* errFile);
void for_to_do(FILE* errFile);
void for_downto_do(FILE* errFile);
void while_statement(FILE* errFile);
void repeat_until(FILE* errFile);

void logical_expression(FILE* errFile);
void and_expression(FILE* errFile);
void comparison(FILE* errFile);
void compound_statement(FILE* errFile);
std::string TokenTypeToString(TypeOfTokens type);

unsigned int IdIdentification(Id IdTable[], Token TokenTable[], unsigned int tokenCount, FILE* errFile);

void Parser(FILE* errFile)
{
    program(errFile);
    fprintf(errFile, "\nNo errors found.\n");
}

void match(TypeOfTokens expectedType, FILE* errFile)
{
    if (TokenTable[pos].type == expectedType)
        pos++;
    else
    {
        fprintf(errFile, "\nSyntax error in line %d : another type of lexeme was expected.\n", TokenTable[pos].line);
        fprintf(errFile, "\nSyntax error: type %s\n", TokenTypeToString(TokenTable[pos].type).c_str());
        fprintf(errFile, "Expected Type: %s ", TokenTypeToString(expectedType).c_str());
        exit(10);
    }
}

void program(FILE* errFile)
{
    match(Mainprogram, errFile);
    match(StartProgram, errFile);
    match(Variable, errFile);
    variable_declaration(errFile);
    match(Semicolon, errFile);
    program_body(errFile);
    match(EndProgram, errFile);
}

void variable_declaration(FILE* errFile)
{
    if (TokenTable[pos].type == Type)
    {
        pos++;
        variable_list(errFile);
    }
}

void variable_list(FILE* errFile)
{
    match(Identifier, errFile);
    while (TokenTable[pos].type == Comma)
    {
        pos++;
        match(Identifier, errFile);
    }
}

void program_body(FILE* errFile)
{
    do
    { 
        statement(errFile);
    } while (TokenTable[pos].type != EndProgram);
}

void statement(FILE* errFile)
{
    switch (TokenTable[pos].type)
    {
    case Input: input(errFile); break;
    case Output: output(errFile); break;
    case If: conditional(errFile); break;
    case Label: label_statement(errFile); break;
    case StartProgram: compound_statement(errFile); break;
    case Goto: goto_statement(errFile); break;
    case For:
    {
        int temp_pos = pos + 1;
        while (TokenTable[temp_pos].type != To && TokenTable[temp_pos].type != DownTo && temp_pos < TokensNum)
        {
            temp_pos++;
        }
        if (TokenTable[temp_pos].type == To)
        {
            for_to_do(errFile);
        }
        else if (TokenTable[temp_pos].type == DownTo)
        {
            for_downto_do(errFile);
        }
        else
        {
            printf("Error: Expected 'To' or 'DownTo' after 'For'\n");
        }
        break;
    }
    case While: while_statement(errFile); break;
    case Exit: pos += 2; break;
    case Continue: pos += 2; break;
    case Repeat: repeat_until(errFile); break;
    default: assignment(errFile); break;
    }
}

void assignment(FILE* errFile)
{
    match(Identifier, errFile);
    match(Assign, errFile);
    arithmetic_expression(errFile);
    match(Semicolon, errFile);
}

void arithmetic_expression(FILE* errFile)
{
    term(errFile);
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        pos++;
        term(errFile);
    }
}

void term(FILE* errFile)
{
    factor(errFile);
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        pos++;
        factor(errFile);
    }
}

void factor(FILE* errFile)
{
    if (TokenTable[pos].type == Identifier)
    {
        match(Identifier, errFile);
    }
    else
        if (TokenTable[pos].type == Number)
        {
            match(Number, errFile);
        }
        else
            if (TokenTable[pos].type == LBraket)
            {
                match(LBraket, errFile);
                arithmetic_expression(errFile);
                match(RBraket, errFile);
            }
            else
            {
                printf("\nSyntax error in line %d : A multiplier was expected.\n", TokenTable[pos].line);
                exit(11);
            }
}

void input(FILE* errFile)
{
    match(Input, errFile);
    match(Identifier, errFile);
    match(Semicolon, errFile);
}

void output(FILE* errFile)
{
    match(Output, errFile);
    if (TokenTable[pos].type == Minus)
    {
        pos++;
        if (TokenTable[pos].type == Number)
        {
            match(Number, errFile);
        }
    }
    else
    {
        arithmetic_expression(errFile);
    }
    match(Semicolon, errFile);
}

void conditional(FILE* errFile)
{
    match(If, errFile);
    logical_expression(errFile); 
    statement(errFile);
    if (TokenTable[pos].type == Else)
    {
        pos++;
        statement(errFile);
    }
}

void goto_statement(FILE* errFile)
{
    match(Goto, errFile);
    if (TokenTable[pos].type == Identifier)
    {
        pos++;
        match(Semicolon, errFile);
    }
    else
    {
        printf("Error: Expected a label after 'goto' at line %d.\n", TokenTable[pos].line);
        exit(1);
    }
}



void label_statement(FILE* errFile)
{
    match(Label, errFile);
}


void for_to_do(FILE* errFile)
{
    match(For, errFile);
    match(Identifier, errFile);
    match(Assign, errFile);
    arithmetic_expression(errFile);
    match(To, errFile);
    arithmetic_expression(errFile);
    match(Do, errFile);
    statement(errFile);
}

void for_downto_do(FILE* errFile)
{
    match(For, errFile);
    match(Identifier, errFile);
    match(Assign, errFile);
    arithmetic_expression(errFile);
    match(DownTo, errFile);
    arithmetic_expression(errFile);
    match(Do, errFile);
    statement(errFile);
}

void while_statement(FILE* errFile)
{
    match(While, errFile); 
    logical_expression(errFile); 

    while (1) 
    {
        if (TokenTable[pos].type == End)
        {
            pos++; 
            match(While, errFile); 
            break; 
        }
        else
        {
            statement(errFile);
            if (TokenTable[pos].type == Semicolon)
            {
                pos++;
            }
        }
    }
}


void repeat_until(FILE* errFile)
{
    match(Repeat, errFile);
    statement(errFile);
    match(Until, errFile);
    logical_expression(errFile);
}

void logical_expression(FILE* errFile)
{
    and_expression(errFile);
    while (TokenTable[pos].type == Or)
    {
        pos++;
        and_expression(errFile);
    }
}

void and_expression(FILE* errFile)
{
    comparison(errFile);
    while (TokenTable[pos].type == And)
    {
        pos++;
        comparison(errFile);
    }
}

void comparison(FILE* errFile)
{
    if (TokenTable[pos].type == Not)
    {
        pos++;
        match(LBraket, errFile);
        logical_expression(errFile);
        match(RBraket, errFile);
    }
    else
        if (TokenTable[pos].type == LBraket)
        {
            pos++;
            logical_expression(errFile);
            match(RBraket, errFile);
        }
        else
        {
            arithmetic_expression(errFile);
            if (TokenTable[pos].type == Greate || TokenTable[pos].type == Less ||
                TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
            {
                pos++;
                arithmetic_expression(errFile);
            }
            else
            {
                fprintf(errFile, "\nSyntax error in line %d : A comparison operation is expected.\n", TokenTable[pos].line);
                exit(12);
            }
        }
}

void compound_statement(FILE* errFile)
{
    match(StartProgram, errFile);
    program_body(errFile);
    match(EndProgram, errFile);
}

unsigned int IdIdentification(Id IdTable[], Token TokenTable[], unsigned int tokenCount, FILE* errFile)
{
    unsigned int idCount = 0;
    unsigned int i = 0;

    while (TokenTable[i++].type != Variable);

    if (TokenTable[i++].type == Type)
    {
        while (TokenTable[i].type != Semicolon)
        {
            if (TokenTable[i].type == Identifier)
            {
                int yes = 0;
                for (unsigned int j = 0; j < idCount; j++)
                {
                    if (!strcmp(TokenTable[i].name, IdTable[j].name))
                    {
                        yes = 1;
                        break;
                    }
                }
                if (yes == 1)
                {
                    printf("\nidentifier \"%s\" is already declared !\n", TokenTable[i].name);
                    return idCount;
                }


                if (idCount < MAX_IDENTIFIER)
                {
                    strcpy_s(IdTable[idCount++].name, TokenTable[i++].name);
                }
                else
                {
                    printf("\nToo many identifiers !\n");
                    return idCount;
                }
            }
            else
                i++;
        }
    }


    for (; i < tokenCount; i++)
    {
        if (TokenTable[i].type == Identifier && TokenTable[i + 1].type != Colon)
        {
            int yes = 0;
            for (unsigned int j = 0; j < idCount; j++)
            {
                if (!strcmp(TokenTable[i].name, IdTable[j].name))
                {
                    yes = 1;
                    break;
                }
            }
            if (yes == 0)
            {
                if (idCount < MAX_IDENTIFIER)
                {
                    strcpy_s(IdTable[idCount++].name, TokenTable[i].name);
                }
                else
                {
                    printf("\nToo many identifiers!\n");
                    return idCount;
                }
            }
        }


    }

    return idCount; 
}

std::string TokenTypeToString(TypeOfTokens type)
{
    switch (type)
    {
    case Mainprogram: return "Mainprogram";
    case StartProgram: return "StartProgram";
    case Variable: return "Variable";
    case Type: return "Type";
    case EndProgram: return "EndProgram";
    case Input: return "Input";
    case Output: return "Output";
    case If: return "If";
    case Else: return "Else";
    case Goto: return "Goto";
    case Label: return "Label";
    case For: return "For";
    case To: return "To";
    case DownTo: return "DownTo";
    case Do: return "Do";
    case While: return "While";
    case Exit: return "Exit";
    case Continue: return "Continue";
    case End: return "End";
    case Repeat: return "Repeat";
    case Until: return "Until";
    case Identifier: return "Identifier";
    case Number: return "Number";
    case Assign: return "Assign";
    case Add: return "Add";
    case Sub: return "Sub";
    case Mul: return "Mul";
    case Div: return "Div";
    case Mod: return "Mod";
    case Equality: return "Equality";
    case NotEquality: return "NotEquality";
    case Greate: return "Greate";
    case Less: return "Less";
    case Not: return "Not";
    case And: return "And";
    case Or: return "Or";
    case LBraket: return "LBraket";
    case RBraket: return "RBraket";
    case Semicolon: return "Semicolon";
    case Colon: return "Colon";
    case Comma: return "Comma";
    case Unknown: return "Unknown";
    default: return "InvalidType";
    }
}