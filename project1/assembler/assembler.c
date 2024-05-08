/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

//int reg[8]; //reg0 ~ reg7;
char* exist_label[MAXLINELENGTH]; //store all labels

void initialize_exist_label (void) {
for (int i = 0; i < MAXLINELENGTH; i++) {
    if (exist_label[i] == NULL) {
        exist_label[i] = malloc(MAXLINELENGTH * sizeof(char));
        exist_label[i][0] = '\0'; // 초기화
    }
}
}

int duplicatedLabel(char *label) // for error checking
{
	for (int i = 0; i < MAXLINELENGTH; i++) 
	{
		if (!strcmp(label, exist_label[i]))
			return 1; //duplicate..
	}
	return 0;
}

int findLabelAddr(char *label)
{
	for (int i = 0; i< MAXLINELENGTH; i++) {
		if (!strcmp(label, exist_label[i])) return i;
	}
	printf("ERROR : can't find such label");
	exit(1);
	return -1; //can't find such label
}

int NonintReg(char *reg)
{
	if(isNumber(reg)) return 0; //integer register arguments
	else return 1; //nonInteger!
}

int RegRangeError(char *reg) //Registers outside the range [0,7]
{
	int Reg = atoi(reg);
	if(Reg < 0 || Reg > 7) return 1;
	else return 0;
} 


int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* here is an example for how to use readAndParse to read a line from
		 inFilePtr */
	//if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		/* reached end of file */
	//}

	/* TODO: Phase-1 label calculation */
	//handling label error && save label

	initialize_exist_label();
	int address = 0;
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		if(label[0] != '\0') {
			if (duplicatedLabel(label))
			{
				printf("ERROR : duplicated definition of labels");
				exit(1);
			}
			if (isNumber(label) || strlen(label) > 6)
			{
				printf("LABEL ERROR");
				exit(1);
			}

			strcpy(exist_label[address], label);
		}
		address++;
	}


	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* TODO: Phase-2 generate machine codes to outfile */

	/* after doing a readAndParse, you may want to do the following to test the
		 opcode */

	int line = 0;
	while(readAndParse(inFilePtr, label, opcode, arg0,
		arg1, arg2))
	{	
		int inst = 0;

		if(!(strcmp(opcode, "noop") == 0 || strcmp(opcode, "halt") == 0 || strcmp(opcode, ".fill") == 0) && (NonintReg(arg0) || NonintReg(arg1))) {
			printf("%d\n", strcmp(opcode, "noop"));
			printf("ERROR : Non-integer register arguments");
			exit(1);
		}	
		if(strcmp(opcode, ".fill") && RegRangeError(arg0) || RegRangeError(arg1)) {
			printf("ERROR : registers outside the range [0,7]");
			exit(1);
		}	

		if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) { //R-type	
			if(NonintReg(arg2)) {
                        	printf("ERROR : Non-integer register arguments");
                        	exit(1);
                		}	
                	if(RegRangeError(arg2)) {
                        	printf("ERROR : registers outside the range [0,7]");
                        	exit(1);
                	}

			if (!strcmp(opcode, "add")) {
				inst |= 0 << 22; //(add)opcode = 000
				//reg[atoi(arg2)] = reg[atoi(arg0)] + reg[atoi(arg1)];
			}
			if (!strcmp(opcode, "nor")) {
				inst |= 1 << 22; //(nor)opcode = 001
				//reg[atoi(arg2)] = ~(reg[atoi(arg0)] | reg[atoi(arg1)]);
			}
			
			inst |= (atoi(arg0) << 19);
			inst |= (atoi(arg1) << 16);
			inst |= (atoi(arg2) << 0);
		} //end of R-type
		
		else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
			
			//opcode set
			if (!strcmp(opcode, "lw")) inst |= 0b10 << 22;
			if (!strcmp(opcode, "sw")) inst |= 0b11 << 22;
			if (!strcmp(opcode, "beq")) inst |= 0b100 << 22;

			int addr = 0;

			if (!isNumber(arg2)) { //symbolic addresses
				addr = findLabelAddr(arg2);
				if (!strcmp(opcode, "beq")) {
					addr = addr - line - 1;
					//addr = (unsigned short)addr;
				}
			}
			else if (isNumber(arg2)) addr = atoi(arg2); //numeric address
			
			int min = -(1 << 15);
    			int max = (1 << 15) - 1;
			if (addr < min || addr > max) {
				printf("ERROR : offsetFields that does not fit in 16 bits");
				exit(1);
			}
			
			inst |= atoi(arg0) << 19;
                        inst |= atoi(arg1) << 16;
                        if (strcmp(opcode, "beq")) inst |= addr << 0;
			else if (!strcmp(opcode, "beq")) inst |= (isNumber(arg2) ? addr : (unsigned short)addr);
		} //end of I-type
	
		else if (!strcmp(opcode, "jalr")) {
			inst |= 0b101 << 22; //opcode

			inst |= atoi(arg0) << 19;
			inst |= atoi(arg1) << 16;
		}

	 	else if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop")) {
			if (!strcmp(opcode, "halt")) inst |= 0b110 << 22;
			if (!strcmp(opcode, "noop")) inst |= 0b111 << 22;
		}
		
		else if (!strcmp(opcode, ".fill")) {
			if (isNumber(arg0)) inst = atoi(arg0);
			else inst = findLabelAddr(arg0);
		}

		else {
			printf("ERROR : Unrecognized opcodes");
			exit(1);
		}
	
		printf("%d\n", inst);
		line++;	
	}

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

