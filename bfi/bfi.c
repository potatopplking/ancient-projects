// Brainfuck interpreter
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Constants
#define MAX_DATA 3000000
#define MAX_CODE 3000000

// Global variables
unsigned char data[MAX_DATA] = {0};
unsigned char code[MAX_CODE];
int mp = 0;
int ip = 0;
FILE* srcCode;
FILE* input;

// Functions for interpreting BF code
void runCode(void)
{
	// Code execution
	while (code[ip] != '\0') {
		switch (code[ip]) {
			case '+':
				data[mp]++;
				break;
			case '-':
				data[mp]--;
				break;
			case '>':
				mp++;
				break;
			case '<':
				mp--;
				break;
			case '.':
				putchar(data[mp]);
				break;
			case ',':
				data[mp] = getc(input);
				break;
			case '[':
				if (!data[mp]) while(code[++ip] != ']');
				break;
			case ']':
				if (data[mp]) while (code[--ip] != '[');
			default:
				// do nothing
				break;
		}
		ip++;
		if (mp >= MAX_DATA) mp = 0;
		if (mp < 0) mp = MAX_DATA;
	}
	putchar('\n');
}

void debugCode(void)
{
	printf("Running in debug mode.\nInstruction pointer = ip\nMemory pointer = mp\n");
	// Code execution in debug mode
	int step = 1;
	while (code[ip] != '\0') {
		printf("\nStep: %d\n",step);
		printf("mp = %d\tip = %d\n",mp,ip);
		switch (code[ip]) {
			case '+':
				printf("Instruction: '+' -> incrementing value memory[%d]\n",mp);
				printf("Before execution:\t");
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				printf("After execution:\t");
				data[mp]++;
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				break;
			case '-':
				printf("Instruction: '-' -> decrementing value memory[%d]\n",mp);
				printf("Before execution:\t");
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				printf("After execution:\t");
				data[mp]--;
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				break;
			case '>':
				printf("Instruction: '>' -> incrementing memory pointer mp = %d\n",mp);
				printf("Before execution:\t");
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				printf("After execution:\t");
				mp++;
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				break;
			case '<':
				printf("Instruction: '<' -> decrementing memory pointer mp = %d\n",mp);
				printf("Before execution:\t");
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				printf("After execution:\t");
				mp--;
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				break;
			case '.':
				printf("Instruction: '.' -> printing memory[%d]\n",mp);
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				break;
			case ',':
				printf("Instruction: ',' -> loading char from input to memory[%d]\n",mp);
				printf("Before execution:\t");
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				printf("After execution:\t");
				data[mp] = getc(input);
				printf("memory[%d] = %d = '%c'\n",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				break;
			case '[':
				printf("Instruction: '[' -> jumping forward to next ']' if memory[%d] == 0\n",mp);
				printf("memory[%d] = %d = '%c' -> ",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				if (!data[mp]) {
					printf("TRUE, jumping to next ']'...\nip = %d -> ",ip);
					while(code[++ip] != ']');
				}
				else {
					printf("FALSE, continuing.\n");
				}
				printf("ip = %d\n",ip);
				break;
			case ']':
				printf("Instruction: ']' -> jumping back to last '[' if memory[%d] != 0\n",mp);
				printf("memory[%d] = %d = '%c' -> ",mp,data[mp],isgraph(data[mp])?data[mp]:' ');
				if (data[mp]) {
					printf("TRUE, jumping to last '['...\nip = %d -> ",ip);
					while (code[--ip] != '[');
				}
				else {
					printf("FALSE, continuing.\n");
				}
				printf("ip = %d\n",ip);
				break;
			default:
				printf("Character '%c' (ASCII: %d) ",isgraph(code[ip])?code[ip]:' ',code[ip]);
				printf("is not Brainfuck operand. Continuing.\n");
		}
		ip++;
		step++;
		if (mp >= MAX_DATA) mp = 0;
		if (mp < 0) mp = MAX_DATA - 1;
	}
	printf("Debug output end.");
	putchar('\n');
}

int main(int argc, char** argv)
{
	int i;
	srcCode = stdin;
	input = stdin;
	void (*runCodeFunc)(void) = runCode;

	// Processing args
	for (i = 1; i != argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'i') {
				if (i < argc) {
					if ((input = fopen(argv[i+1],"r")) == NULL) {
						fprintf(stderr,"bfi: input file '%s' couldn't be opened\n",argv[i+1]);
						exit(1);
					}
				}
				else {
					fprintf(stderr,"bfi: argument to '-i' is missing\n");
					exit(1);
				}
				i++;
			}
			else if (argv[i][1] == 'h') {
				printf("bfi: Brainfuck Interpreter\nUsage: bfi [-d] [-i USER_INPUT] [SOURCE_CODE_FILE]\n");
				printf(" -d\tRun in debug mode\n");
				printf("USER_INPUT: overrides input given to programme. Default value is stdin.\n");
				printf("SOURCE_CODE_FILE: your own stuff. If not specified, bfi will take input from stdin.\n");
				printf("Notice: When loading source code from stdin, ");
				printf("programme continues to load until EOF (Ctrl-D) is detected\n");
				exit(0);
			}
			else if (argv[i][1] == 'd') {
				runCodeFunc = debugCode;
			}
			else {
				fprintf(stderr,"bfi: unknown option '-%c'\n",argv[i][1]);
				exit(1);
			}
		}
		else {
			if ((srcCode = fopen(argv[i],"r")) == NULL) {
				fprintf(stderr,"bfi: source code file '%s' couldn't be opened (doesn't exist?)\n",argv[i]);
				exit(1);
			}
		}
	}

	// Loading code from src file 
	if (fread(code,1,MAX_CODE,srcCode) == 0) exit(1);
	// Clearing the EOF so that we can read from it again
	clearerr(srcCode);
	runCodeFunc();
	return 0;
}
