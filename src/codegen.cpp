#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "translator.h"

// таблиця лексем
extern Token* TokenTable;
// кількість лексем
extern unsigned int TokensNum;

// таблиця ідентифікаторів
extern Id* IdTable;
// кількість ідентифікаторів
extern unsigned int IdNum;

static int pos = 2;

// набір функцій для рекурсивного спуску 
// на кожне правило - окрема функція

void gen_variable_declaration(FILE* outFile);
void gen_variable_list(FILE* outFile);
void gen_program_body(FILE* outFile);
void gen_statement(FILE* outFile);
void gen_assignment(FILE* outFile);
void gen_arithmetic_expression(FILE* outFile);
void gen_term(FILE* outFile);
void gen_factor(FILE* outFile);
void gen_input(FILE* outFile);
void gen_output(FILE* outFile);
void gen_conditional(FILE* outFile);

void gen_goto_statement(FILE* outFile);
void gen_label_statement(FILE* outFile);
void gen_for_to_do(FILE* outFile);
void gen_for_downto_do(FILE* outFile);
void gen_while_statement(FILE* outFile);
void gen_repeat_until(FILE* outFile);

void gen_logical_expression(FILE* outFile);
void gen_and_expression(FILE* outFile);
void gen_comparison(FILE* outFile);
void gen_compound_statement(FILE* outFile);

void generateCCode(FILE* outFile)
{
    fprintf(outFile, "#include <stdio.h>\n");
    fprintf(outFile, "#include <stdlib.h>\n\n");
    fprintf(outFile, "int main() \n{\n");
    gen_variable_declaration(outFile);
    fprintf(outFile, ";\n");
    pos++;
    gen_program_body(outFile);
    fprintf(outFile, "   system(\"pause\");\n ");
    fprintf(outFile, "   return 0;\n");
    fprintf(outFile, "}\n");
}


// <оголошення змінних> = [<тип даних> <список змінних>]
void gen_variable_declaration(FILE* outFile)
{
    if (TokenTable[pos + 1].type == Type)
    {
        fprintf(outFile, "   int ");
        pos++;
        pos++;
        gen_variable_list(outFile);
    }
}

// <список змінних> = <ідентифікатор> { ',' <ідентифікатор> }
void gen_variable_list(FILE* outFile)
{
    fprintf(outFile, TokenTable[pos++].name);
    while (TokenTable[pos].type == Comma)
    {
        fprintf(outFile, ", ");
        pos++;
        fprintf(outFile, TokenTable[pos++].name);
    }
}

// <тіло програми> = <оператор> ';' { <оператор> ';' }
void gen_program_body(FILE* outFile)
{
    while (pos < TokensNum && TokenTable[pos].type != EndProgram)
    {
        gen_statement(outFile);
    }

    if (pos >= TokensNum || TokenTable[pos].type != EndProgram)
    {
        printf("Error: 'EndProgram' token not found or unexpected end of tokens.\n");
        exit(1);
    }
}

// <оператор> = <присвоїння> | <ввід> | <вивід> | <умовний оператор> | <складений оператор>
void gen_statement(FILE* outFile)
{
    switch (TokenTable[pos].type)
    {
    case Input: gen_input(outFile); break;
    case Output: gen_output(outFile); break;
    case If: gen_conditional(outFile); break;
    case StartProgram: gen_compound_statement(outFile); break;
    case Goto: gen_goto_statement(outFile); break;
    case Label: gen_label_statement(outFile); break;
    case For:
    {
        int temp_pos = pos + 1;

        while (TokenTable[temp_pos].type != To && TokenTable[temp_pos].type != DownTo && temp_pos < TokensNum)
        {
            temp_pos++;
        }

        if (TokenTable[temp_pos].type == To)
        {
            gen_for_to_do(outFile);
        }
        else if (TokenTable[temp_pos].type == DownTo)
        {
            gen_for_downto_do(outFile);
        }
        else
        {
            printf("Error: Expected 'To' or 'DownTo' after 'For'\n");
        }
    }
    break;
    case While: gen_while_statement(outFile); break;
    case Exit:
        fprintf(outFile, "      break;\n");
        pos += 2; 
        break;

    case Continue:
        fprintf(outFile, "      continue;\n");
        pos += 2; 
        break;
    case Repeat: gen_repeat_until(outFile); break;
    default: gen_assignment(outFile);
    }
}

// <присвоїння> = <ідентифікатор> ':=' <арифметичний вираз>
void gen_assignment(FILE* outFile)
{
    fprintf(outFile, "   ");
    fprintf(outFile, TokenTable[pos++].name);
    fprintf(outFile, " = ");
    pos++;
    gen_arithmetic_expression(outFile);
    pos++;
    fprintf(outFile, ";\n");
}

// <арифметичний вираз> = <доданок> { ('+' | '-') <доданок> }
void gen_arithmetic_expression(FILE* outFile)
{
    gen_term(outFile);
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        if (TokenTable[pos].type == Add)
            fprintf(outFile, " + ");
        else
            fprintf(outFile, " - ");
        pos++;
        gen_term(outFile);
    }
}

// <доданок> = <множник> { ('*' | '/') <множник> }
void gen_term(FILE* outFile)
{
    gen_factor(outFile);
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        if (TokenTable[pos].type == Mul)
            fprintf(outFile, " * ");
        if (TokenTable[pos].type == Div)
            fprintf(outFile, " / ");
        if (TokenTable[pos].type == Mod)
            fprintf(outFile, " %% ");
        pos++;
        gen_factor(outFile);
    }
}

// <множник> = <≥дентиф≥катор> | <число> | '(' <арифметичний вираз> ')'
void gen_factor(FILE* outFile)
{
    if (TokenTable[pos].type == Identifier || TokenTable[pos].type == Number)
        fprintf(outFile, TokenTable[pos++].name);
    else
        if (TokenTable[pos].type == LBraket)
        {
            fprintf(outFile, "(");
            pos++;
            gen_arithmetic_expression(outFile);
            fprintf(outFile, ")");
            pos++;
        }
}

// <вв≥д> = 'input' <≥дентиф≥катор>
void gen_input(FILE* outFile)
{
    fprintf(outFile, "   printf(\"Enter ");
    fprintf(outFile, TokenTable[pos + 1].name);
    fprintf(outFile, ":\");\n");
    fprintf(outFile, "   scanf(\"%%d\", &");
    pos++;
    fprintf(outFile, TokenTable[pos++].name);
    fprintf(outFile, ");\n");
    pos++;
}

// <вив≥д> = 'output' <≥дентиф≥катор>
void gen_output(FILE* outFile)
{
    pos++;

    if (TokenTable[pos].type == Minus && TokenTable[pos + 1].type == Number)
    {
        fprintf(outFile, "   printf(\"%%d\\n\", -%s);\n", TokenTable[pos + 1].name);
        pos += 2;
    }
    else
    {
        fprintf(outFile, "   printf(\"%%d\\n\", ");
        gen_arithmetic_expression(outFile);
        fprintf(outFile, ");\n");
    }

    if (TokenTable[pos].type == Semicolon)
    {
        pos++;
    }
    else
    {
        printf("Error: Expected a semicolon at the end of 'Output' statement.\n");
        exit(1);
    }
}




// <умовний оператор> = 'if' <лог≥чний вираз> 'then' <оператор> [ 'else' <оператор> ]
void gen_conditional(FILE* outFile)
{
    fprintf(outFile, "   if (");
    pos++;
    gen_logical_expression(outFile);
    fprintf(outFile, ")\n");
    gen_statement(outFile);
    if (TokenTable[pos].type == Else)
    {
        fprintf(outFile, "   else\n");
        pos++;
        gen_statement(outFile);
    }
}

void gen_goto_statement(FILE* outFile)
{
    fprintf(outFile, "   goto %s;\n", TokenTable[pos + 1].name);
    pos += 3;
}

void gen_label_statement(FILE* outFile)
{
    fprintf(outFile, "%s:\n", TokenTable[pos].name); 
    pos++;
}

void gen_for_to_do(FILE* outFile)
{
    int temp_pos = pos + 1; 

    const char* loop_var = TokenTable[temp_pos].name;
    temp_pos += 2; 

    fprintf(outFile, "   for (int %s = ", loop_var);
    pos = temp_pos; 
    gen_arithmetic_expression(outFile); 
    fprintf(outFile, "; ");

    while (TokenTable[pos].type != To && pos < TokensNum)
    {
        pos++;
    }

    if (TokenTable[pos].type == To)
    {
        pos++; 
        fprintf(outFile, "%s <= ", loop_var);
        gen_arithmetic_expression(outFile); 
    }
    else
    {
        printf("Error: Expected 'To' in For-To loop\n");
        return;
    }

    fprintf(outFile, "; %s++)\n", loop_var);

    if (TokenTable[pos].type == Do)
    {
        pos++; 
    }
    else
    {
        printf("Error: Expected 'Do' after 'To' clause\n");
        return;
    }

    gen_statement(outFile);
}
void gen_for_downto_do(FILE* outFile)
{
    int temp_pos = pos + 1;

    const char* loop_var = TokenTable[temp_pos].name;
    temp_pos += 2;

    fprintf(outFile, "   for (int %s = ", loop_var);
    pos = temp_pos; 
    gen_arithmetic_expression(outFile); 
    fprintf(outFile, "; ");

    while (TokenTable[pos].type != DownTo && pos < TokensNum)
    {
        pos++;
    }

    if (TokenTable[pos].type == DownTo)
    {
        pos++;

        fprintf(outFile, "%s >= ", loop_var);
        gen_arithmetic_expression(outFile); 
    }
    else
    {
        printf("Error: Expected 'Downto' in For-Downto loop\n");
        return;
    }

    fprintf(outFile, "; %s--)\n", loop_var);

    if (TokenTable[pos].type == Do)
    {
        pos++; 
    }
    else
    {
        printf("Error: Expected 'Do' after 'Downto' clause\n");
        return;
    }

    gen_statement(outFile);
}

void gen_while_statement(FILE* outFile)
{
    fprintf(outFile, "   while (");
    pos++;
    gen_logical_expression(outFile);
    fprintf(outFile, ")\n   {\n");

    while (pos < TokensNum)
    {
        if (TokenTable[pos].type == End && TokenTable[pos + 1].type == While)
        {
            pos += 2;
            break;    
        }
        else
        {
            gen_statement(outFile); 
            if (TokenTable[pos].type == Semicolon)
            {
                pos++;
            }
        }
    }

    fprintf(outFile, "   }\n");
}



void gen_repeat_until(FILE* outFile)
{
    fprintf(outFile, "   do\n");
    pos++; 
    do
    {
        gen_statement(outFile);     
    } while (TokenTable[pos].type != Until);
    fprintf(outFile, "   while (");
    pos++; 
    gen_logical_expression(outFile); 
    fprintf(outFile, ");\n");
}

// <лог≥чний вираз> = <вираз ≤> { '|' <вираз ≤> }
void gen_logical_expression(FILE* outFile)
{
    gen_and_expression(outFile);
    while (TokenTable[pos].type == Or)
    {
        fprintf(outFile, " || ");
        pos++;
        gen_and_expression(outFile);
    }
}

// <вираз ≤> = <пор≥вн¤нн¤> { '&' <пор≥вн¤нн¤> }
void gen_and_expression(FILE* outFile)
{
    gen_comparison(outFile);
    while (TokenTable[pos].type == And)
    {
        fprintf(outFile, " && ");
        pos++;
        gen_comparison(outFile);
    }
}

// <пор≥вн¤нн¤> = <операц≥¤ пор≥вн¤нн¤> | С!С С(С <лог≥чний вираз> С)С | С(С <лог≥чний вираз> С)С
// <операц≥¤ пор≥вн¤нн¤> = <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
// <менше-б≥льше> = С>С | С<С | С=С | С<>С
void gen_comparison(FILE* outFile)
{
    if (TokenTable[pos].type == Not)
    {
        // ¬ар≥ант: ! (<лог≥чний вираз>)
        fprintf(outFile, "!(");
        pos++;
        pos++;
        gen_logical_expression(outFile);
        fprintf(outFile, ")");
        pos++;
    }
    else
        if (TokenTable[pos].type == LBraket)
        {
            // ¬ар≥ант: ( <лог≥чний вираз> )
            fprintf(outFile, "(");
            pos++;
            gen_logical_expression(outFile);
            fprintf(outFile, ")");
            pos++;
        }
        else
        {
            // ¬ар≥ант: <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
            gen_arithmetic_expression(outFile);
            if (TokenTable[pos].type == Greate || TokenTable[pos].type == Less ||
                TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
            {
                switch (TokenTable[pos].type)
                {
                case Greate: fprintf(outFile, " > "); break;
                case Less: fprintf(outFile, " < "); break;
                case Equality: fprintf(outFile, " == "); break;
                case NotEquality: fprintf(outFile, " != "); break;
                }
                pos++;
                gen_arithmetic_expression(outFile);
            }
        }
}

// <складений оператор> = 'start' <т≥ло програми> 'stop'
void gen_compound_statement(FILE* outFile)
{
    fprintf(outFile, "   {\n");
    pos++;
    gen_program_body(outFile);
    fprintf(outFile, "   }\n");
    pos++;
}



