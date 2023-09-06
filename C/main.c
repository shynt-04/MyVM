/*LC-3 Architecture*/

#include<stdio.h>
#include<stdint.h>

/*
    The LC-3 has (1<<16)=65536 memory locations each of which stores a 16-bit value
*/
#define MEMORY_MAX (1 << 16)
uint16_t memory[MEMORY_MAX];

// REGISTERS
/*
    A register is a slot for storing a single value on the CPU.
    For the CPU to work with a piece of data, it has to be stored in one of the registers.
    The LC-3 has 10 registers, each of which is 16 bits.
    - 8 general purposes (R0-R7)
    - 1 program counter (PC)
    - 1 condition flags (COND)
*/
enum 
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_R1,
    R_COND, /* condition flags */
    R_COUNT
};
uint16_t reg[R_COUNT];

// CONDITION FLAGS
/*
    The R_COND register stores condition flags which provide information about the most recently executed calculation.
    This allows programs to check logical condition such as: if (x > 0) { ... }.
    The LC-3 use only 3 condition flags which indicates sign of the previous calculation.
*/

enum {
    FL_POS = 1 << 0, /* positive */
    FL_ZRO = 1 << 1, /* zero */
    FL_NEG = 1 << 2 /* negative */
};

// INSTRUCTION
/*
    An instruction is a command which tells the CPU to do some fundamental task.
    Intruction include: 
    - an opcode which indicates the kind of task to perform
    - a set of parameters which provides input to the task being performed
    Each instruction is 16 bits long, with the left 4 bits storing the opcode, the rest for parameters 

*/

// OPCODE
enum {
    OP_BR = 0, /* branch */
    OP_ADD, /* add */
    OP_LD, /* load */
    OP_ST, /* store*/
    OP_JSR, /* jump register */
    OP_AND, /* bitwise and */
    OP_LDR, /* load register */
    OP_STR, /* store register */
    OP_RTI, /* unused */
    OP_NOT, /* bitwise not */
    OP_LDI, /* load indirect*/
    OP_STI, /* store indirect */
    OP_JMP, /* jump */
    OP_RES, /* reserved (unused) */
    OP_LEA, /* load effective address */
    OP_TRAP /* execute trap */
};

int main(int argc, const char *argv[]) {
    // LOAD STATEMENT
    if (argc < 2) {
        /* show usage */
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    for (int j = 1; j < argc; ++ j) {
        if (!read_image(argv[j])) {
            printf("Failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }


    // SETUP

    // MAIN LOOP
    /* since exactly one conditon flag should be given at any given time, set the Z flag*/
    reg[R_COND] = FL_ZRO;

    /* set the PC to starting position, which default is 0x3000 */
    enum {
        PC_START = 0x3000,
    };
    reg[R_PC] = PC_START;

    int running = 1;
    while (running) {
        /* FETCH */
        uint16_t instr = mem_read(reg[R_PC] ++);
        uint16_t op = instr >> 12; /* remember the left 4 bits is for opcode*/

        switch (op) {
            case OP_ADD:
                break;
            case OP_AND:
                break;
            case OP_NOT:
                break;
            case OP_BR:
                break;
            case OP_JMP:
                break;
            case OP_JSR:
                break;
            case OP_LD:
                break;
            case OP_LDI:
                break;
            case OP_LDR:
                break;
            case OP_LEA:
                break;
            case OP_ST:
                break;
            case OP_STI:
                break;
            case OP_STR:
                break;
            case OP_TRAP:
                break;
            case OP_RES:
            case OP_RTI:
            default:
                break;
        }
    }
    

}