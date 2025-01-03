#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translator.h"


// Рекурсивна функція для генерації коду з AST
void generateCodefromAST(ASTNode* node, FILE* outFile)
{
    if (node == NULL)
        return;

    switch (node->nodetype)
    {
    case program_node:
        fprintf(outFile, "#include <stdio.h>\n#include <stdlib.h>\n\nint main() \n{\n");
        generateCodefromAST(node->left, outFile);  // Оголошення змінних
        generateCodefromAST(node->right, outFile); // Тіло програми
        fprintf(outFile, "   system(\"pause\");\n ");
        fprintf(outFile, "   return 0;\n}\n");
        break;

    case var_node:
        // Якщо є права частина (інші змінні), додаємо коми і генеруємо для них код
        if (node->right != NULL)
        {
            //fprintf(outFile, ", ");
            generateCodefromAST(node->right, outFile);  // Рекурсивно генеруємо код для інших змінних
        }
        fprintf(outFile, "   int ");  // Виводимо тип змінних (в даному випадку int)     
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, ";\n");  // Завершуємо оголошення змінних
        break;

    case id_node:
        fprintf(outFile, "%s", node->name);
        break;

    case num_node:
        fprintf(outFile, "%s", node->name);
        break;

    case assign_node:
        fprintf(outFile, "   ");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " = ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ";\n");
        break;

    case add_node:
        fprintf(outFile, "(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " + ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ")");
        break;

    case sub_node:
        fprintf(outFile, "(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " - ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ")");
        break;

    case mul_node:
        fprintf(outFile, "(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " * ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ")");
        break;

    case mod_node:
        fprintf(outFile, "(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " %% ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ")");
        break;

    case div_node:
        fprintf(outFile, "(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " / ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ")");
        break;

    case input_node:
        fprintf(outFile, "   printf(\"Enter ");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, ":\");\n");
        fprintf(outFile, "   scanf(\"%%d\", &");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, ");\n");
        break;

    case output_node: 
        fprintf(outFile, "   printf(\"%%d\\n\", ");
        generateCodefromAST(node->left, outFile);

        fprintf(outFile, ");\n");
        break;





    case if_node:
        fprintf(outFile, "   if (");
        generateCodefromAST(node->left, outFile); 
        fprintf(outFile, ") \n");
        generateCodefromAST(node->right->left, outFile); 
        if (node->right->right != NULL)
        { 
            fprintf(outFile, "   else\n");
            generateCodefromAST(node->right->right, outFile);
        }
        break;

    case goto_node:
        fprintf(outFile, "   goto %s;\n", node->left->name);
        break;

    case label_node:
        fprintf(outFile, "%s:\n", node->name); 
        break;


    case for_to_node:
        fprintf(outFile, "   for (int ");
        generateCodefromAST(node->left->left, outFile); 
        fprintf(outFile, " = ");
        generateCodefromAST(node->left->right, outFile); 
        fprintf(outFile, "; ");
        generateCodefromAST(node->left->left, outFile); 
        fprintf(outFile, " <= ");
        generateCodefromAST(node->right->left, outFile); 
        fprintf(outFile, "; ");
        generateCodefromAST(node->left->left, outFile); 
        fprintf(outFile, "++)\n");
        generateCodefromAST(node->right->right, outFile); 
        break;

    case for_downto_node:
        fprintf(outFile, "   for (int ");
        generateCodefromAST(node->left->left, outFile); 
        fprintf(outFile, " = ");
        generateCodefromAST(node->left->right, outFile); 
        fprintf(outFile, "; ");
        generateCodefromAST(node->left->left, outFile); 
        fprintf(outFile, " >= ");
        generateCodefromAST(node->right->left, outFile); 
        fprintf(outFile, "; ");
        generateCodefromAST(node->left->left, outFile); 
        fprintf(outFile, "--)\n");
        generateCodefromAST(node->right->right, outFile);
        break;

    case while_node:
        fprintf(outFile, "   while (");
        generateCodefromAST(node->left, outFile); 
        fprintf(outFile, ")\n");
        fprintf(outFile, "   {\n");
        generateCodefromAST(node->right, outFile); 
        fprintf(outFile, "   }\n");
        break;

    case exit_while_node:
        fprintf(outFile, "   break;\n");
        break;

    case continue_while_node:
        fprintf(outFile, "   continue;\n");
        break;


    case repeat_until_node:
        fprintf(outFile, "   do\n");
        generateCodefromAST(node->left, outFile); 
        fprintf(outFile, "   while (");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ");\n");
        break;

    case or_node:
        fprintf(outFile, "(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " || ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ")");
        break;

    case and_node:
        fprintf(outFile, "(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, " && ");
        generateCodefromAST(node->right, outFile);
        fprintf(outFile, ")");
        break;

    case not_node:
        fprintf(outFile, "!(");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, ")");
        break;

    case cmp_node:
        generateCodefromAST(node->left, outFile);
        if (!strcmp(node->name, "=="))
            fprintf(outFile, " == ");
        else if (!strcmp(node->name, "<>"))          
            fprintf(outFile, " != ");
            else  if (!strcmp(node->name, ">>"))
            fprintf(outFile, " > ");
            else if (!strcmp(node->name, "<<"))
            fprintf(outFile, " < ");
            else
                fprintf(outFile, " %s ", node->name);
        generateCodefromAST(node->right, outFile);
        break;

    case statement_node:
        generateCodefromAST(node->left, outFile);
        if (node->right != NULL)
            generateCodefromAST(node->right, outFile);
        break;

    case compount_node:
        fprintf(outFile, "   {\n");
        generateCodefromAST(node->left, outFile);
        fprintf(outFile, "   }\n");
        break;

    default:
        fprintf(stderr, "Unknown node type: %d\n", node->nodetype);
        break;
    }
}