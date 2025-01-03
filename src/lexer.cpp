#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "translator.h"
#include <locale>

// функція отримує лексеми з вхідного файлу F і записує їх у таблицю лексем TokenTable 
// результат функції - кількість лексем
unsigned int GetTokens(FILE* F, Token TokenTable[], FILE* errFile)
{
	States state = Start;
	Token TempToken;
	// кількість лексем
	unsigned int NumberOfTokens = 0;
	char ch, buf[32];
	int line = 1;

	// читання першого символу з файлу 
	ch = getc(F);

	// пошук лексем
	while (1)
	{
		switch (state)
		{
			// стан Start - початок виділення чергової лексеми 
			// якщо поточний символ маленька літера, то переходимо до стану Letter
			// якщо поточний символ цифра, то переходимо до стану Digit
			// якщо поточний символ пробіл, символ табуляції або переходу на новий рядок, то переходимо до стану Separators
			// якщо поточний символ EOF (ознака кінця файлу), то переходимо до стану EndOfFile
			// якщо поточний символ відмінний від попередніх, то переходимо до стану Another
		case Start:
		{
			if (ch == EOF)
				state = EndOfFile;
			else
				if ((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A') || ch == '_')
					state = Letter;
				else
					if (ch <= '9' && ch >= '0')
						state = Digit;
					else
						if (ch == ' ' || ch == '\t' || ch == '\n')
							state = Separators;
						else
							if (ch == '#')
								state = SComment;
							else
								state = Another;
			break;
		}

		// стан Finish - кінець виділення чергової лексеми і запис лексеми у таблицю лексем
		case Finish:
		{
			if (NumberOfTokens < MAX_TOKENS)
			{
				TokenTable[NumberOfTokens++] = TempToken;
				if (ch != EOF)
					state = Start;
				else
					state = EndOfFile;
			}
			else
			{
				printf("\n\t\t\ttoo many tokens !!!\n");
				return NumberOfTokens - 1;
			}
			break;
		}

		// стан EndOfFile - кінець файлу, можна завершувати пошук лексем
		case EndOfFile:
		{
			return NumberOfTokens;
		}

		// стан Letter - поточний символ - маленька літера, поточна лексема - ключове слово або ідентифікатор
		case Letter:
		{
			buf[0] = ch;
			int j = 1;

			ch = getc(F);

			while (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
				(ch >= '0' && ch <= '9') || ch == '_' || ch == ':' || ch == '-') && j < 31)
			{
				buf[j++] = ch;
				ch = getc(F);
			}
			buf[j] = '\0'; 

			TypeOfTokens temp_type = Unknown;

			if (!strcmp(buf, "end"))
			{
				char next_buf[16];
				int next_j = 0;

				while (ch == ' ' || ch == '\t')
				{
					ch = getc(F);
				}

				while (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) && next_j < 31)
				{
					next_buf[next_j++] = ch;
					ch = getc(F);
				}
				next_buf[next_j] = '\0';

				if (!strcmp(next_buf, "while"))
				{
					temp_type = End;
					strcpy_s(TempToken.name, buf);
					TempToken.type = temp_type;
					TempToken.value = 0;
					TempToken.line = line;
					TokenTable[NumberOfTokens++] = TempToken;

					temp_type = While;
					strcpy_s(TempToken.name, next_buf);
					TempToken.type = temp_type;
					TempToken.value = 0;
					TempToken.line = line;
					TokenTable[NumberOfTokens++] = TempToken;

					state = Start;
					break;
				}
			}


			if (!strcmp(buf, "startprogram"))	temp_type = Mainprogram;
			else if (!strcmp(buf, "startblok"))		temp_type = StartProgram;
			else if (!strcmp(buf, "variable"))		temp_type = Variable;
			else if (!strcmp(buf, "longint"))	temp_type = Type;
			else if (!strcmp(buf, "endblok"))		temp_type = EndProgram;
			else if (!strcmp(buf, "read"))		temp_type = Input;
			else if (!strcmp(buf, "write"))	temp_type = Output;

			else if (!strcmp(buf, "add"))		temp_type = Add;
			else if (!strcmp(buf, "sub"))		temp_type = Sub;

			else if (!strcmp(buf, "if"))		temp_type = If;
			else if (!strcmp(buf, "else"))		temp_type = Else;
			else if (!strcmp(buf, "goto"))		temp_type = Goto;
			else if (!strcmp(buf, "for"))		temp_type = For;
			else if (!strcmp(buf, "to"))		temp_type = To;
			else if (!strcmp(buf, "downto"))	temp_type = DownTo;
			else if (!strcmp(buf, "do"))		temp_type = Do;
			else if (!strcmp(buf, "exit"))      temp_type = Exit;
			else if (!strcmp(buf, "while"))     temp_type = While;
			else if (!strcmp(buf, "continue"))  temp_type = Continue;
			else if (!strcmp(buf, "repeat"))	temp_type = Repeat;
			else if (!strcmp(buf, "until"))		temp_type = Until;
			if (temp_type == Unknown && TokenTable[NumberOfTokens - 1].type == Goto)
			{
				temp_type = Identifier;
			}
			else if (buf[strlen(buf) - 1] == ':')
			{
				buf[strlen(buf) - 1] = '\0'; 
				temp_type = Label;
			}
			else if ((buf[0] >= 'a' && buf[0] <= 'z') && (strlen(buf) == 16))
			{				
				bool valid = true;

				for (int i = 1; i < 16; i++)
				{
					if (!(buf[i] >= 'a' && buf[i] <= 'z') && !(buf[i] >= '0' && buf[i] <= '9'))
					{
						valid = false; 
						break;
					}
				}
				if (valid)
				{
					temp_type = Identifier; 
				}
			}
			strcpy_s(TempToken.name, buf);
			TempToken.type = temp_type;
			TempToken.value = 0;
			TempToken.line = line;
			if (temp_type == Unknown)
			{
				fprintf(errFile, "Lexical Error: line %d, lexem %s is Unknown\n", line, TempToken.name);
			}
			state = Finish;
			break;
		}

		case Digit:
		{
			buf[0] = ch;
			int j = 1;

			ch = getc(F);

			while ((ch <= '9' && ch >= '0') && j < 15)
			{
				buf[j++] = ch;
				ch = getc(F);
			}
			buf[j] = '\0';

			strcpy_s(TempToken.name, buf);
			TempToken.type = Number;
			TempToken.value = atoi(buf);
			TempToken.line = line;
			state = Finish;
			break;
		}

		case Separators:
		{
			if (ch == '\n')
				line++;

			ch = getc(F);

			state = Start;
			break;
		}

		case SComment:
		{
			ch = getc(F);
			if (ch == '#')
				state = Comment;
			break;
		}

		case Comment:
		{
			while (ch != '\n' && ch != EOF)
			{
				ch = getc(F);
			}
			if (ch == EOF)
			{
				state = EndOfFile;
				break;
			}
			state = Start;
			break;
		}

		case Another:
		{
			switch (ch)
			{

			case '(':
			{
				strcpy_s(TempToken.name, "(");
				TempToken.type = LBraket;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case  ')':
			{
				strcpy_s(TempToken.name, ")");
				TempToken.type = RBraket;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case ';':
			{
				strcpy_s(TempToken.name, ";");
				TempToken.type = Semicolon;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case  ',':
			{
				strcpy_s(TempToken.name, ",");
				TempToken.type = Comma;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case ':':
			{
				char next = getc(F);				
					strcpy_s(TempToken.name, ":");
					TempToken.type = Colon; 
					ungetc(next, F);        
				
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case '-':
			{
				strcpy_s(TempToken.name, "-");
				TempToken.type = Minus;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case '*':
			{

				strcpy_s(TempToken.name, "*");
				TempToken.type = Mul;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;

				break;
			}

			case '/':
			{
				strcpy_s(TempToken.name, "/");
				TempToken.type = Div;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case '%':
			{
				strcpy_s(TempToken.name, "%");
				TempToken.type = Mod;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case '&':
			{
				strcpy_s(TempToken.name, "&");
				TempToken.type = And;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;			
				break;
			}

			case '|':
			{
				strcpy_s(TempToken.name, "|");
				TempToken.type = Or;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case '!':
			{
				strcpy_s(TempToken.name, "!");
				TempToken.type = Not;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}

			case '<':
			{
				ch = getc(F);
				if (ch == '=')
				{
					ch = getc(F);
					if (ch == '=')
					{
						strcpy_s(TempToken.name, "<==");
						TempToken.type = Assign;
						TempToken.value = 0;
						TempToken.line = line;
						ch = getc(F);
						state = Finish;
					}
				}
				else if (ch == '<')
				{
					strcpy_s(TempToken.name, "<<");
					TempToken.type = Less;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F);
					state = Finish;			
				}
				else if (ch == '>')
				{
					strcpy_s(TempToken.name, "<>");
					TempToken.type = NotEquality;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F);
					state = Finish;
				}
				break;
			}

			case '>':
			{
				ch = getc(F);
				if (ch == '>')
				{
					strcpy_s(TempToken.name, ">>");
					TempToken.type = Greate;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F);
					state = Finish;
					break;
				}
			}

			case '=':
			{
				ch = getc(F);
				if (ch == '=')
				{
					strcpy_s(TempToken.name, "==");
					TempToken.type = Equality;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F);
					state = Finish;
					break;
				}
			}


			default:
			{
				TempToken.name[0] = ch;
				TempToken.name[1] = '\0';
				TempToken.type = Unknown;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			}
		}
		}
	}
}

void PrintTokens(Token TokenTable[], unsigned int TokensNum)
{
	char type_tokens[16];
	printf("\n\n---------------------------------------------------------------------------\n");
	printf("|             TOKEN TABLE                                                 |\n");
	printf("---------------------------------------------------------------------------\n");
	printf("| line number |      token      |    value   | token code | type of token |\n");
	printf("---------------------------------------------------------------------------");
	for (unsigned int i = 0; i < TokensNum; i++)
	{
		switch (TokenTable[i].type)
		{
		case Mainprogram:
			strcpy_s(type_tokens, "MainProgram");
			break;
		case StartProgram:
			strcpy_s(type_tokens, "StartProgram");
			break;
		case Variable:
			strcpy_s(type_tokens, "Variable");
			break;
		case Type:
			strcpy_s(type_tokens, "Integer");
			break;
		case Identifier:
			strcpy_s(type_tokens, "Identifier");
			break;
		case EndProgram:
			strcpy_s(type_tokens, "EndProgram");
			break;
		case Input:
			strcpy_s(type_tokens, "Input");
			break;
		case Output:
			strcpy_s(type_tokens, "Output");
			break;
		case If:
			strcpy_s(type_tokens, "If");
			break;	
		case Else:
			strcpy_s(type_tokens, "Else");
			break;
		case Assign:
			strcpy_s(type_tokens, "Assign");
			break;
		case Add:
			strcpy_s(type_tokens, "Add");
			break;
		case Sub:
			strcpy_s(type_tokens, "Sub");
			break;
		case Mul:
			strcpy_s(type_tokens, "Mul");
			break;
		case Div:
			strcpy_s(type_tokens, "Div");
			break;
		case Mod:
			strcpy_s(type_tokens, "Mod");
			break;
		case Equality:
			strcpy_s(type_tokens, "Equality");
			break;
		case NotEquality:
			strcpy_s(type_tokens, "NotEquality");
			break;
		case Greate:
			strcpy_s(type_tokens, "Greate");
			break;
		case Less:
			strcpy_s(type_tokens, "Less");
			break;
		case Not:
			strcpy_s(type_tokens, "Not");
			break;
		case And:
			strcpy_s(type_tokens, "And");
			break;
		case Or:
			strcpy_s(type_tokens, "Or");
			break;
		case LBraket:
			strcpy_s(type_tokens, "LBraket");
			break;
		case RBraket:
			strcpy_s(type_tokens, "RBraket");
			break;
		case Number:
			strcpy_s(type_tokens, "Number");
			break;
		case Semicolon:
			strcpy_s(type_tokens, "Semicolon");
			break;
		case Comma:
			strcpy_s(type_tokens, "Comma");
			break;
		case Goto:
			strcpy_s(type_tokens, "Goto");
			break;
		case For:
			strcpy_s(type_tokens, "For");
			break;
		case To:
			strcpy_s(type_tokens, "To");
			break;
		case DownTo:
			strcpy_s(type_tokens, "DownTo");
			break;
		case Do:
			strcpy_s(type_tokens, "Do");
			break;
		case While:
			strcpy_s(type_tokens, "While");
			break;
		case Exit:
			strcpy_s(type_tokens, "Exit");
			break;
		case Continue:
			strcpy_s(type_tokens, "Continue");
			break;
		case End:
			strcpy_s(type_tokens, "End");
			break;
		case Repeat:
			strcpy_s(type_tokens, "Repeat");
			break;
		case Until:
			strcpy_s(type_tokens, "Until");
			break;
		case Label:
			strcpy_s(type_tokens, "Label");
			break;
		case Unknown:
		default:
			strcpy_s(type_tokens, "Unknown");
			break;
		}

		printf("\n|%12d |%16s |%11d |%11d | %-13s |\n",
			TokenTable[i].line,
			TokenTable[i].name,
			TokenTable[i].value,
			TokenTable[i].type,
			type_tokens);
		printf("---------------------------------------------------------------------------");
	}
	printf("\n");
}

void PrintTokensToFile(char* FileName, Token TokenTable[], unsigned int TokensNum)
{
	FILE* F;
	if ((fopen_s(&F, FileName, "wt")) != 0)
	{
		printf("Error: Can not create file: %s\n", FileName);
		return;
	}
	char type_tokens[16];
	fprintf(F, "---------------------------------------------------------------------------\n");
	fprintf(F, "|             TOKEN TABLE                                                 |\n");
	fprintf(F, "---------------------------------------------------------------------------\n");
	fprintf(F, "| line number |      token      |    value   | token code | type of token |\n");
	fprintf(F, "---------------------------------------------------------------------------");
	for (unsigned int i = 0; i < TokensNum; i++)
	{
		switch (TokenTable[i].type)
		{
		case Mainprogram:
			strcpy_s(type_tokens, "MainProgram");
			break;
		case StartProgram:
			strcpy_s(type_tokens, "StartProgram");
			break;
		case Variable:
			strcpy_s(type_tokens, "Variable");
			break;
		case Type:
			strcpy_s(type_tokens, "Integer");
			break;
		case Identifier:
			strcpy_s(type_tokens, "Identifier");
			break;
		case EndProgram:
			strcpy_s(type_tokens, "EndProgram");
			break;
		case Input:
			strcpy_s(type_tokens, "Input");
			break;
		case Output:
			strcpy_s(type_tokens, "Output");
			break;
		case If:
			strcpy_s(type_tokens, "If");
			break;
		case Else:
			strcpy_s(type_tokens, "Else");
			break;
		case Assign:
			strcpy_s(type_tokens, "Assign");
			break;
		case Add:
			strcpy_s(type_tokens, "Add");
			break;
		case Sub:
			strcpy_s(type_tokens, "Sub");
			break;
		case Mul:
			strcpy_s(type_tokens, "Mul");
			break;
		case Div:
			strcpy_s(type_tokens, "Div");
			break;
		case Mod:
			strcpy_s(type_tokens, "Mod");
			break;
		case Equality:
			strcpy_s(type_tokens, "Equality");
			break;
		case NotEquality:
			strcpy_s(type_tokens, "NotEquality");
			break;
		case Greate:
			strcpy_s(type_tokens, "Greate");
			break;
		case Less:
			strcpy_s(type_tokens, "Less");
			break;
		case Not:
			strcpy_s(type_tokens, "Not");
			break;
		case And:
			strcpy_s(type_tokens, "And");
			break;
		case Or:
			strcpy_s(type_tokens, "Or");
			break;
		case LBraket:
			strcpy_s(type_tokens, "LBraket");
			break;
		case RBraket:
			strcpy_s(type_tokens, "RBraket");
			break;
		case Number:
			strcpy_s(type_tokens, "Number");
			break;
		case Semicolon:
			strcpy_s(type_tokens, "Semicolon");
			break;
		case Comma:
			strcpy_s(type_tokens, "Comma");
			break;
		case Goto:
			strcpy_s(type_tokens, "Goto");
			break;
		case For:
			strcpy_s(type_tokens, "For");
			break;
		case To:
			strcpy_s(type_tokens, "To");
			break;
		case DownTo:
			strcpy_s(type_tokens, "DownTo");
			break;
		case Do:
			strcpy_s(type_tokens, "Do");
			break;
		case While:
			strcpy_s(type_tokens, "While");
			break;
		case Exit:
			strcpy_s(type_tokens, "Exit");
			break;
		case Continue:
			strcpy_s(type_tokens, "Continue");
			break;
		case End:
			strcpy_s(type_tokens, "End");
			break;
		case Repeat:
			strcpy_s(type_tokens, "Repeat");
			break;
		case Until:
			strcpy_s(type_tokens, "Until");
			break;
		case Label:
			strcpy_s(type_tokens, "Label");
			break;
		case Unknown:
		default:
			strcpy_s(type_tokens, "Unknown");
			break;
		}

		fprintf(F, "\n|%12d |%16s |%11d |%11d | %-13s |\n",
			TokenTable[i].line,
			TokenTable[i].name,
			TokenTable[i].value,
			TokenTable[i].type,
			type_tokens);
		fprintf(F, "---------------------------------------------------------------------------");
	}
	fclose(F);
}
