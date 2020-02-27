#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mu-mips.h"

/***************************************************************/
/* Print out a list of commands available                                                                  */
/***************************************************************/
void help() {        
	printf("------------------------------------------------------------------\n\n");
	printf("\t**********MU-MIPS Help MENU**********\n\n");
	printf("sim\t-- simulate program to completion \n");
	printf("run <n>\t-- simulate program for <n> instructions\n");
	printf("rdump\t-- dump register values\n");
	printf("reset\t-- clears all registers/memory and re-loads the program\n");
	printf("input <reg> <val>\t-- set GPR <reg> to <val>\n");
	printf("mdump <start> <stop>\t-- dump memory from <start> to <stop> address\n");
	printf("high <val>\t-- set the HI register to <val>\n");
	printf("low <val>\t-- set the LO register to <val>\n");
	printf("print\t-- print the program loaded into memory\n");
	printf("?\t-- display help menu\n");
	printf("quit\t-- exit the simulator\n\n");
	printf("------------------------------------------------------------------\n\n");
}

/***************************************************************/
/* Read a 32-bit word from memory                                                                            */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) &&  ( address <= MEM_REGIONS[i].end) ) {
			uint32_t offset = address - MEM_REGIONS[i].begin;
			return (MEM_REGIONS[i].mem[offset+3] << 24) |
					(MEM_REGIONS[i].mem[offset+2] << 16) |
					(MEM_REGIONS[i].mem[offset+1] <<  8) |
					(MEM_REGIONS[i].mem[offset+0] <<  0);
		}
	}
	return 0;
}

/***************************************************************/
/* Write a 32-bit word to memory                                                                                */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
	int i;
	uint32_t offset;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) && (address <= MEM_REGIONS[i].end) ) {
			offset = address - MEM_REGIONS[i].begin;

			MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
			MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
			MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
			MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
		}
	}
}

/***************************************************************/
/* Execute one cycle                                                                                                              */
/***************************************************************/
void cycle() {                                                
	handle_instruction();
	CURRENT_STATE = NEXT_STATE;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/* Simulate MIPS for n cycles                                                                                       */
/***************************************************************/
void run(int num_cycles) {                                      
	
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped\n\n");
		return;
	}

	printf("Running simulator for %d cycles...\n\n", num_cycles);
	int i;
	for (i = 0; i < num_cycles; i++) {
		if (RUN_FLAG == FALSE) {
			printf("Simulation Stopped.\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/* simulate to completion                                                                                               */
/***************************************************************/
void runAll() {                                                     
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped.\n\n");
		return;
	}

	printf("Simulation Started...\n\n");
	while (RUN_FLAG){
		cycle();
	}
	printf("Simulation Finished.\n\n");
}

/***************************************************************/ 
/* Dump a word-aligned region of memory to the terminal                              */
/***************************************************************/
void mdump(uint32_t start, uint32_t stop) {          
	uint32_t address;

	printf("-------------------------------------------------------------\n");
	printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
	printf("-------------------------------------------------------------\n");
	printf("\t[Address in Hex (Dec) ]\t[Value]\n");
	for (address = start; address <= stop; address += 4){
		printf("\t0x%08x (%d) :\t0x%08x\n", address, address, mem_read_32(address));
	}
	printf("\n");
}

/***************************************************************/
/* Dump current values of registers to the teminal                                              */   
/***************************************************************/
void rdump() {                               
	int i; 
	printf("-------------------------------------\n");
	printf("Dumping Register Content\n");
	printf("-------------------------------------\n");
	printf("# Instructions Executed\t: %u\n", INSTRUCTION_COUNT);
	printf("PC\t: 0x%08x\n", CURRENT_STATE.PC);
	printf("-------------------------------------\n");
	printf("[Register]\t[Value]\n");
	printf("-------------------------------------\n");
	for (i = 0; i < MIPS_REGS; i++){
		printf("[R%d]\t: 0x%08x\n", i, CURRENT_STATE.REGS[i]);
	}
	printf("-------------------------------------\n");
	printf("[HI]\t: 0x%08x\n", CURRENT_STATE.HI);
	printf("[LO]\t: 0x%08x\n", CURRENT_STATE.LO);
	printf("-------------------------------------\n");
}

/***************************************************************/
/* Read a command from standard input.                                                               */  
/***************************************************************/
void handle_command() {                         
	char buffer[20];
	uint32_t start, stop, cycles;
	uint32_t register_no;
	int register_value;
	int hi_reg_value, lo_reg_value;

	printf("MU-MIPS SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'S':
		case 's':
			runAll(); 
			break;
		case 'M':
		case 'm':
			if (scanf("%x %x", &start, &stop) != 2){
				break;
			}
			mdump(start, stop);
			break;
		case '?':
			help();
			break;
		case 'Q':
		case 'q':
			printf("**************************\n");
			printf("Exiting MU-MIPS! Good Bye...\n");
			printf("**************************\n");
			exit(0);
		case 'R':
		case 'r':
			if (buffer[1] == 'd' || buffer[1] == 'D'){
				rdump();
			}else if(buffer[1] == 'e' || buffer[1] == 'E'){
				reset();
			}
			else {
				if (scanf("%d", &cycles) != 1) {
					break;
				}
				run(cycles);
			}
			break;
		case 'I':
		case 'i':
			if (scanf("%u %i", &register_no, &register_value) != 2){
				break;
			}
			CURRENT_STATE.REGS[register_no] = register_value;
			NEXT_STATE.REGS[register_no] = register_value;
			break;
		case 'H':
		case 'h':
			if (scanf("%i", &hi_reg_value) != 1){
				break;
			}
			CURRENT_STATE.HI = hi_reg_value; 
			NEXT_STATE.HI = hi_reg_value; 
			break;
		case 'L':
		case 'l':
			if (scanf("%i", &lo_reg_value) != 1){
				break;
			}
			CURRENT_STATE.LO = lo_reg_value;
			NEXT_STATE.LO = lo_reg_value;
			break;
		case 'P':
		case 'p':
			print_program(); 
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}

/***************************************************************/
/* reset registers/memory and reload program                                                    */
/***************************************************************/
void reset() {   
	int i;
	/*reset registers*/
	for (i = 0; i < MIPS_REGS; i++){
		CURRENT_STATE.REGS[i] = 0;
	}
	CURRENT_STATE.HI = 0;
	CURRENT_STATE.LO = 0;
	
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
	
	/*load program*/
	load_program();
	
	/*reset PC*/
	INSTRUCTION_COUNT = 0;
	CURRENT_STATE.PC =  MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/***************************************************************/
/* Allocate and set memory to zero                                                                            */
/***************************************************************/
void init_memory() {                                           
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		MEM_REGIONS[i].mem = malloc(region_size);
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
}

/**************************************************************/
/* load program into memory                                                                                      */
/**************************************************************/
void load_program() {                   
	FILE * fp;
	int i, word;
	uint32_t address;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */

	i = 0;
	while( fscanf(fp, "%x\n", &word) != EOF ) {
		address = MEM_TEXT_BEGIN + i;
		mem_write_32(address, word);
		printf("writing 0x%08x into address 0x%08x (%d)\n", word, address, address);
		i += 4;
	}
	PROGRAM_SIZE = i/4;
	printf("Program loaded into memory.\n%d words written into memory.\n\n", PROGRAM_SIZE);
	fclose(fp);
}

/************************************************************/
/* decode and execute instruction                                                                     */ 
/************************************************************/





int parseReg(char * reg){


    if(!strncmp(reg, "$zero", 10)){
        return 0b00000;
    }
    if(!strncmp(reg, "$r0", 10)){  // 4
        return 0b00100;
    }
    if(!strncmp(reg, "$r1", 10)){
        return 0b00101;
    }
    if(!strncmp(reg, "$r2", 10)){
        return 0b00110;
    }
    if(!strncmp(reg, "$r3", 10)){
        return 0b00111;
    }
    if(!strncmp(reg, "$r4", 10)){ // 8
        return 0b01000;
    }
    if(!strncmp(reg, "$r5", 10)){ 
        return 0b01001;
    }
    if(!strncmp(reg, "$r6", 10)){
        return 0b01010;
    }
    if(!strncmp(reg, "$r7", 10)){
        return 0b01011;
    }
    if(!strncmp(reg, "$r8", 10)){ //12 
        return 0b01100;
    }
    if(!strncmp(reg, "$r9", 10)){
        return 0b01101;
    }
    if(!strncmp(reg, "$r10", 10)){
        return 0b01110;
    }
    if(!strncmp(reg, "$r11", 10)){
        return 0b01011;
    }
    if(!strncmp(reg, "$r12", 10)){
        return 0b01100;
    }
    if(!strncmp(reg, "$r13", 10)){
        return 0b01101;
    }
    if(!strncmp(reg, "$r14", 10)){
        return 0b01110;
    }
    if(!strncmp(reg, "$r15", 10)){
        return 0b01111;
    }
}


// thx codeFTW 
// https://codeforwin.org/2015/08/c-program-to-convert-hexadecimal-to-binary-number-system.html
int fromBinary(const char *s) {

    char bin[65] = "";
    int i = 2;
    for(i=2; s[i]!='\0'; i++)
    {
        switch(s[i])
        {
            case '0':
                strcat(bin, "0000");
                break;
            case '1':
                strcat(bin, "0001");
                break;
            case '2':
                strcat(bin, "0010");
                break;
            case '3':
                strcat(bin, "0011");
                break;
            case '4':
                strcat(bin, "0100");
                break;
            case '5':
                strcat(bin, "0101");
                break;
            case '6':
                strcat(bin, "0110");
                break;
            case '7':
                strcat(bin, "0111");
                break;
            case '8':
                strcat(bin, "1000");
                break;
            case '9':
                strcat(bin, "1001");
                break;
            case 'a':
            case 'A':
                strcat(bin, "1010");
                break;
            case 'b':
            case 'B':
                strcat(bin, "1011");
                break;
            case 'c':
            case 'C':
                strcat(bin, "1100");
                break;
            case 'd':
            case 'D':
                strcat(bin, "1101");
                break;
            case 'e':
            case 'E':
                strcat(bin, "1110");
                break;
            case 'f':
            case 'F':
                strcat(bin, "1111");
                break;
            default:
                printf("fail %c\n", s[i]);
            
        }
    }

  return (int) strtol(bin, NULL, 2) & 0b1111111111111111;
}

int parseArg(char * arg, int off){
    
    
    //  printf("stuff-%s  %d-\n", arg, off);

	if(off == 2){
		return fromBinary(arg);
	}
    if(strchr(arg, '(') != NULL){
        const char paraOpen[2] = "(";
        if(off == 1){
        char * token;
        token = strtok(arg, paraOpen);
         return fromBinary(token);
        }else{
        char *p = strtok(arg,paraOpen);
        p = strtok(NULL,")");
        return parseReg(p);
        }
    }

 
    const char s[2] = ",";
    arg = strtok(arg, s);

    
    // hex parse 
    if(arg[0] != '$' && arg[1] != '\0'){
        return (int)strtol(arg, NULL, 16) & 0xFFFF;
    }

    // 0 parse 
    if(arg[1] == '\0'){
        printf("%s\n", arg);
        return atoi(arg) & 0xFFFF;
    }

    // register parse .......
        return parseReg(arg);
}



void writeInstruction(int op){
    char instructionString[10];
    sprintf(instructionString, "%x\n", op);
    FILE * instFile = fopen("instructions.in", "a");
    fputs(instructionString, instFile);
	fclose(instFile);
}

void doWork(char * name){

    fclose(fopen("instructions.in", "w")); // clear file 
    FILE *fp;
    char *word = (char*)malloc(40);
    fp = fopen("inst.s","r"); //read only
    if(fp == NULL){
        return;
    }
    
    while(1){
        if(fscanf(fp, "%s", word) == EOF) break;
		printf("%s", word);
        // case addiu
        if(!strncmp(word, "addiu", 10)){
            // addiu rt,rs,imm
            int addiuOp = 0b001001;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0); // rt
            if(fscanf(fp, "%s", word) == EOF) break;
            int rs = parseArg(word,0); // rs
            if(fscanf(fp, "%s", word) == EOF) break;
            int imm = parseArg(word,0); // imm
            addiuOp = (addiuOp << 5) | rs;
            addiuOp = (addiuOp << 5) | rt;
            addiuOp = (addiuOp << 16) | imm;
            printf("%x", addiuOp); // write to file ... 
            writeInstruction(addiuOp);

        }
        
        // case lui
        if(!strncmp(word, "lui", 10)){
            
            // lui rt, imm
            int addiuOp = 0b001111;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0); // rt
            if(fscanf(fp, "%s", word) == EOF) break;
            int imm = parseArg(word,0); // imm
            addiuOp = (addiuOp << 5) | 0b00000;
            addiuOp = (addiuOp << 5) | rt;
            addiuOp = (addiuOp << 16) | imm;


            printf("%x", addiuOp); // write to file ...
			writeInstruction(addiuOp);

        }
        
        //lw  ex-> 8C690000 -> LW $r9, 0x0($r3)
        if(!strncmp(word, "lw", 10)){
            
            // lw 
            printf("lw");

			char wrdCopy[50];
            int lw = 0b100011;

            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0); 
            if(fscanf(fp, "%s", word) == EOF) break;
            strcpy (wrdCopy, word);
            int off = parseArg(word,1); // offset 
            int bs = parseArg(wrdCopy,0); // bs 
            lw = (lw << 5) | bs;
            lw = (lw << 5) | rt;
            lw = (lw << 16) | off;
            printf("%x\n",lw);
			writeInstruction(lw);

        }
        
        
        
        // bgez  -- check 
        if(!strncmp(word, "bge", 10)){
             printf("bge");

            // bgez rs,offset 
            int regimm = 0b000001;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rs = parseArg(word,0); 
            int bgez = 0b00001;
            if(fscanf(fp, "%s", word) == EOF) break;
            int off = parseArg(word,2); // offset 
            regimm = (regimm << 5) | rs;
            regimm = (regimm << 5) | bgez;
            regimm = (regimm << 16) | off;


            printf("%x", regimm); // write to file ...
			writeInstruction(regimm);

        }
        
        
        
        
        // sub
          if(!strncmp(word, "sub", 10)){
            printf("sub");
            // sub rd, rs, rt 
            int special = 0b000000;
            int subOp = 0b100010;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rd = parseArg(word,0); 
            if(fscanf(fp, "%s", word) == EOF) break;
            int rs = parseArg(word,0);  
            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0);  

            special = (special << 5) | rs;
            special = (special << 5) | rt;
            special = (special << 5) | rd;
            special = (special << 5) | 0b00000;
            special = (special << 6) | subOp;


            printf("%x", special); // write to file ...
			writeInstruction(special);

        }
        
        
        
                
        // bgtz  -- check 
        if(!strncmp(word, "bgtz", 10)){
            printf("bgtz");

            // beq rs, rt, offset 
            int bgtz = 0b000111;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rs = parseArg(word,0); 
            if(fscanf(fp, "%s", word) == EOF) break;
            int off = parseArg(word,0); // offset 
            bgtz = (bgtz << 5) | rs;
            bgtz = (bgtz << 5) | 0b00000;
            bgtz = (bgtz << 16) | off;

            printf("%x", bgtz); // write to file ...
            writeInstruction(bgtz);

        }

        // bne  -- check 
        if(!strncmp(word, "bne", 10)){
            printf("bne");

            // bne rs, rt, offset 
            int bne = 0b000101;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rs = parseArg(word,0); 
            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0); 
			if(fscanf(fp, "%s", word) == EOF) break;
            int off = parseArg(word,0); // offset 
            bne = (bne << 5) | rs;
            bne = (bne << 5) | rt;
            bne = (bne << 16) | off;
            printf("%x", bne); // write to file ...
            writeInstruction(bne);
        }
        
        //sw  AC650000 for sw $r5, 0x0($r3)
        if(!strncmp(word, "sw", 10)){
            // sw rt, off(bs) 
            char wrdCopy[50];
            int sw = 0b101011;

            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0); 
            if(fscanf(fp, "%s", word) == EOF) break;
            strcpy (wrdCopy, word);
            int off = parseArg(word,1); // offset 
            int bs = parseArg(wrdCopy,0); // bs 
            sw = (sw << 5) | bs;
            sw = (sw << 5) | rt;
            sw = (sw << 16) | off;
            printf("%x",sw);
            writeInstruction(sw);

        }

        
        // add
        
        
          if(!strncmp(word, "add", 10)){
            // ADD rd, rs, rt 
            int special = 0b000000;
            int addOp = 0b100000;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rd = parseArg(word,0); 
            if(fscanf(fp, "%s", word) == EOF) break;
            int rs = parseArg(word,0);  
            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0);  

            special = (special << 5) | rs;
            special = (special << 5) | rt;
            special = (special << 5) | rd;
            special = (special << 5) | 0b00000;
            special = (special << 6) | addOp;
            printf("%x", special); // write to file ...
            writeInstruction(special);

        }
		 // addu
        
        
          if(!strncmp(word, "addu", 10)){
            // ADD rd, rs, rt 
            int special = 0b000000;
            int adduOp = 0b100001;
            if(fscanf(fp, "%s", word) == EOF) break;
            int rd = parseArg(word,0); 
            if(fscanf(fp, "%s", word) == EOF) break;
            int rs = parseArg(word,0);  
            if(fscanf(fp, "%s", word) == EOF) break;
            int rt = parseArg(word,0);  

            special = (special << 5) | rs;
            special = (special << 5) | rt;
            special = (special << 5) | rd;
            special = (special << 5) | 0b00000;
            special = (special << 6) | adduOp;
            printf("%x", special); // write to file ...
            writeInstruction(special);
        }
        
    }
    fclose(fp);
    return;
}











void handle_instruction()
{
	/*IMPLEMENT THIS*/
	/* execute one instruction at a time. Use/update CURRENT_STATE and and NEXT_STATE, as necessary.*/



	
	
	uint32_t instruction, opcode, function, rs, rt, rd, sa, immediate, target;
	uint64_t product, p1, p2;
	
	uint32_t addr, data;
	
	int branch_jump = FALSE;
	
	printf("[0x%x]\t", CURRENT_STATE.PC);
	
	instruction = mem_read_32(CURRENT_STATE.PC);
	
	opcode = (instruction & 0xFC000000) >> 26;
	function = instruction & 0x0000003F;
	rs = (instruction & 0x03E00000) >> 21;
	rt = (instruction & 0x001F0000) >> 16;
	rd = (instruction & 0x0000F800) >> 11;
	sa = (instruction & 0x000007C0) >> 6;
	immediate = instruction & 0x0000FFFF;
	target = instruction & 0x03FFFFFF;
	
	if(opcode == 0x00){
		switch(function){
			case 0x00: //SLL
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x02: //SRL
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x03: //SRA 
				if ((CURRENT_STATE.REGS[rt] & 0x80000000) == 1)
				{
					NEXT_STATE.REGS[rd] =  ~(~CURRENT_STATE.REGS[rt] >> sa );
				}
				else{
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x08: //JR
				NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
				branch_jump = TRUE;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x09: //JALR
				NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
				NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
				branch_jump = TRUE;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x0C: //SYSCALL
				if(CURRENT_STATE.REGS[2] == 0xa){
					RUN_FLAG = FALSE;
					print_instruction(CURRENT_STATE.PC);
				}
				break;
			case 0x10: //MFHI
				NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x11: //MTHI
				NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x12: //MFLO
				NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x13: //MTLO
				NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x18: //MULT
				if ((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x80000000){
					p1 = 0xFFFFFFFF00000000 | CURRENT_STATE.REGS[rs];
				}else{
					p1 = 0x00000000FFFFFFFF & CURRENT_STATE.REGS[rs];
				}
				if ((CURRENT_STATE.REGS[rt] & 0x80000000) == 0x80000000){
					p2 = 0xFFFFFFFF00000000 | CURRENT_STATE.REGS[rt];
				}else{
					p2 = 0x00000000FFFFFFFF & CURRENT_STATE.REGS[rt];
				}
				product = p1 * p2;
				NEXT_STATE.LO = (product & 0X00000000FFFFFFFF);
				NEXT_STATE.HI = (product & 0XFFFFFFFF00000000)>>32;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x19: //MULTU
				product = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
				NEXT_STATE.LO = (product & 0X00000000FFFFFFFF);
				NEXT_STATE.HI = (product & 0XFFFFFFFF00000000)>>32;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x1A: //DIV 
				if(CURRENT_STATE.REGS[rt] != 0)
				{
					NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
					NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x1B: //DIVU
				if(CURRENT_STATE.REGS[rt] != 0)
				{
					NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
					NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x20: //ADD
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x21: //ADDU 
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] + CURRENT_STATE.REGS[rs];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x22: //SUB
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x23: //SUBU
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x24: //AND
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x25: //OR
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x26: //XOR
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x27: //NOR
				NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x2A: //SLT
				if(CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]){
					NEXT_STATE.REGS[rd] = 0x1;
				}
				else{
					NEXT_STATE.REGS[rd] = 0x0;
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			default:
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
	else{
		switch(opcode){
			case 0x01:
				if(rt == 0x00000){ //BLTZ
					if((CURRENT_STATE.REGS[rs] & 0x80000000) > 0){
						NEXT_STATE.PC = CURRENT_STATE.PC + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000)<<2 : (immediate & 0x0000FFFF)<<2);
						branch_jump = TRUE;
					}
					print_instruction(CURRENT_STATE.PC);
				}
				else if(rt == 0x00001){ //BGEZ
					if((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x0){
						NEXT_STATE.PC = CURRENT_STATE.PC + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000)<<2 : (immediate & 0x0000FFFF)<<2);
						branch_jump = TRUE;
					}
					print_instruction(CURRENT_STATE.PC);
				}
				break;
			case 0x02: //J
				NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
				branch_jump = TRUE;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x03: //JAL
				NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
				NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
				branch_jump = TRUE;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x04: //BEQ
				if(CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]){
					NEXT_STATE.PC = CURRENT_STATE.PC + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000)<<2 : (immediate & 0x0000FFFF)<<2);
					branch_jump = TRUE;
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x05: //BNE
				if(CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]){
					NEXT_STATE.PC = CURRENT_STATE.PC + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000)<<2 : (immediate & 0x0000FFFF)<<2);
					branch_jump = TRUE;
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x06: //BLEZ
				if((CURRENT_STATE.REGS[rs] & 0x80000000) > 0 || CURRENT_STATE.REGS[rs] == 0){
					NEXT_STATE.PC = CURRENT_STATE.PC +  ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000)<<2 : (immediate & 0x0000FFFF)<<2);
					branch_jump = TRUE;
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x07: //BGTZ
				if((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x0 || CURRENT_STATE.REGS[rs] != 0){
					NEXT_STATE.PC = CURRENT_STATE.PC +  ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000)<<2 : (immediate & 0x0000FFFF)<<2);
					branch_jump = TRUE;
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x08: //ADDI
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF));
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x09: //ADDIU
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF));
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x0A: //SLTI
				if ( (  (int32_t)CURRENT_STATE.REGS[rs] - (int32_t)( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF))) < 0){
					NEXT_STATE.REGS[rt] = 0x1;
				}else{
					NEXT_STATE.REGS[rt] = 0x0;
				}
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x0C: //ANDI
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & (immediate & 0x0000FFFF);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x0D: //ORI
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | (immediate & 0x0000FFFF);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x0E: //XORI
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ (immediate & 0x0000FFFF);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x0F: //LUI
				NEXT_STATE.REGS[rt] = immediate << 16;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x20: //LB
				data = mem_read_32( CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF)) );
				NEXT_STATE.REGS[rt] = ((data & 0x000000FF) & 0x80) > 0 ? (data | 0xFFFFFF00) : (data & 0x000000FF);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x21: //LH
				data = mem_read_32( CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF)) );
				NEXT_STATE.REGS[rt] = ((data & 0x0000FFFF) & 0x8000) > 0 ? (data | 0xFFFF0000) : (data & 0x0000FFFF);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x23: //LW
				NEXT_STATE.REGS[rt] = mem_read_32( CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF)) );
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x28: //SB
				addr = CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF));
				data = mem_read_32( addr);
				data = (data & 0xFFFFFF00) | (CURRENT_STATE.REGS[rt] & 0x000000FF);
				mem_write_32(addr, data);
				print_instruction(CURRENT_STATE.PC);				
				break;
			case 0x29: //SH
				addr = CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF));
				data = mem_read_32( addr);
				data = (data & 0xFFFF0000) | (CURRENT_STATE.REGS[rt] & 0x0000FFFF);
				mem_write_32(addr, data);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x2B: //SW
				addr = CURRENT_STATE.REGS[rs] + ( (immediate & 0x8000) > 0 ? (immediate | 0xFFFF0000) : (immediate & 0x0000FFFF));
				mem_write_32(addr, CURRENT_STATE.REGS[rt]);
				print_instruction(CURRENT_STATE.PC);
				break;
			default:
				// put more things here
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
	
	if(!branch_jump){
		NEXT_STATE.PC = CURRENT_STATE.PC + 4;
	}
}


/************************************************************/
/* Initialize Memory                                                                                                    */ 
/************************************************************/
void initialize() { 
	init_memory();
	CURRENT_STATE.PC = MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/************************************************************/
/* Print the program loaded into memory (in MIPS assembly format)    */ 
/************************************************************/
void print_program(){
	int i;
	uint32_t addr;
	
	for(i=0; i<PROGRAM_SIZE; i++){
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		print_instruction(addr);
	}
}

/************************************************************/
/* Print the instruction at given memory address (in MIPS assembly format)    */
/************************************************************/
void print_instruction(uint32_t addr){
	uint32_t instruction, opcode, function, rs, rt, rd, sa, immediate, target;
	
	instruction = mem_read_32(addr);
	
	opcode = (instruction & 0xFC000000) >> 26;
	function = instruction & 0x0000003F;
	rs = (instruction & 0x03E00000) >> 21;
	rt = (instruction & 0x001F0000) >> 16;
	rd = (instruction & 0x0000F800) >> 11;
	sa = (instruction & 0x000007C0) >> 6;
	immediate = instruction & 0x0000FFFF;
	target = instruction & 0x03FFFFFF;
	
	if(opcode == 0x00){
		/*R format instructions here*/
		
		switch(function){
			case 0x00:
				printf("SLL $r%u, $r%u, 0x%x\n", rd, rt, sa);
				break;
			case 0x02:
				printf("SRL $r%u, $r%u, 0x%x\n", rd, rt, sa);
				break;
			case 0x03:
				printf("SRA $r%u, $r%u, 0x%x\n", rd, rt, sa);
				break;
			case 0x08:
				printf("JR $r%u\n", rs);
				break;
			case 0x09:
				if(rd == 31){
					printf("JALR $r%u\n", rs);
				}
				else{
					printf("JALR $r%u, $r%u\n", rd, rs);
				}
				break;
			case 0x0C:
				printf("SYSCALL\n");
				break;
			case 0x10:
				printf("MFHI $r%u\n", rd);
				break;
			case 0x11:
				printf("MTHI $r%u\n", rs);
				break;
			case 0x12:
				printf("MFLO $r%u\n", rd);
				break;
			case 0x13:
				printf("MTLO $r%u\n", rs);
				break;
			case 0x18:
				printf("MULT $r%u, $r%u\n", rs, rt);
				break;
			case 0x19:
				printf("MULTU $r%u, $r%u\n", rs, rt);
				break;
			case 0x1A:
				printf("DIV $r%u, $r%u\n", rs, rt);
				break;
			case 0x1B:
				printf("DIVU $r%u, $r%u\n", rs, rt);
				break;
			case 0x20:
				printf("ADD $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x21:
				printf("ADDU $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x22:
				printf("SUB $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x23:
				printf("SUBU $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x24:
				printf("AND $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x25:
				printf("OR $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x26:
				printf("XOR $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x27:
				printf("NOR $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x2A:
				printf("SLT $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			default:
				printf("Instruction is not implemented!\n");
				break;
		}
	}
	else{
		switch(opcode){
			case 0x01:
				if(rt == 0){
					printf("BLTZ $r%u, 0x%x\n", rs, immediate<<2);
				}
				else if(rt == 1){
					printf("BGEZ $r%u, 0x%x\n", rs, immediate<<2);
				}
				break;
			case 0x02:
				printf("J 0x%x\n", (addr & 0xF0000000) | (target<<2));
				break;
			case 0x03:
				printf("JAL 0x%x\n", (addr & 0xF0000000) | (target<<2));
				break;
			case 0x04:
				printf("BEQ $r%u, $r%u, 0x%x\n", rs, rt, immediate<<2);
				break;
			case 0x05:
				printf("BNE $r%u, $r%u, 0x%x\n", rs, rt, immediate<<2);
				break;
			case 0x06:
				printf("BLEZ $r%u, 0x%x\n", rs, immediate<<2);
				break;
			case 0x07:
				printf("BGTZ $r%u, 0x%x\n", rs, immediate<<2);
				break;
			case 0x08:
				printf("ADDI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x09:
				printf("ADDIU $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0A:
				printf("SLTI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0C:
				printf("ANDI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0D:
				printf("ORI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0E:
				printf("XORI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0F:
				printf("LUI $r%u, 0x%x\n", rt, immediate);
				break;
			case 0x20:
				printf("LB $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x21:
				printf("LH $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x23:
				printf("LW $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x28:
				printf("SB $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x29:
				printf("SH $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x2B:
				printf("SW $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			default:
				printf("Instruction is not implemented!\n");
				break;
		}
	}
}

/***************************************************************/
/* main                                                                                                                                   */
/***************************************************************/
int main(int argc, char *argv[]) {      

	printf("\n**************************\n");
	printf("Welcome to MU-MIPS SIM...\n");
	printf("**************************\n\n");

	
	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}
	doWork(argv[2]);

	strcpy(prog_file, argv[1]);
	initialize();
	load_program();
	help();
	while (1){
		handle_command();
	}
	return 0;
}
