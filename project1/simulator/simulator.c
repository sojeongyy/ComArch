/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY]; //16bits
    int reg[NUMREGS]; //reg0 ~ reg7
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int opcode, arg0, arg1, arg2, offset = 0;
void memRead(int code)
{
    opcode = (code >> 22) & 0x7; //24-22bits
    arg0 = (code >> 19) & 0x7;
    arg1 = (code >> 16) & 0x7;
    arg2 = code & 0xFFFF;
    offset = convertNum(arg2);
}
  

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

		
    for (int i = 0; i < NUMREGS; i++)
	state.reg[i] = 0;
    
    state.pc = 0;    
	
    int ishalt = 0;

    while(1)
    {
	if (ishalt) break;
	memRead(state.mem[state.pc]);
	printState(&state);
	switch(opcode) {
	    
	    case 0: //add
		state.reg[arg2] = state.reg[arg0] + state.reg[arg1];
		state.pc++;
		break;

	    case 1: //nor
		state.reg[arg2] = ~(state.reg[arg0] | state.reg[arg1]);
		state.pc++;
		break;

	    case 2: //lw
		state.reg[arg1] = state.mem[state.reg[arg0] + offset];
		state.pc++;
		break;

	    case 3: //sw
		state.mem[state.reg[arg0] + offset] = state.reg[arg1];
		state.pc++;
		break;

	    case 4: //beq
		if (state.reg[arg0] == state.reg[arg1]) 
		    state.pc = state.pc + 1 + offset;
		else state.pc++;
		break;
	
	    case 5: //jalr
		state.reg[arg1] = state.pc + 1;
		state.pc = state.reg[arg0];
		break;

	    case 6: //halt
		//printf("NOTE : the machine halted!\n");
		ishalt = 1;
		break;

	    case 7: //noop
		break;
	} //switch end
    } //while end	

    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
