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

void update_flags(uint16_t r) {
    if (reg[r] == 0) {
        reg[R_COND] = FL_ZRO;
    } else if (reg[r] >> 15) { // left-most bit = 1 => negative, read Two's complement
        reg[R_COND] = FL_NEG;
    } else {
        reg[R_COND] = FL_POS;
    }
    
}

uint16_t sign_extend(uint16_t x, int bit_count) {
    /* 
        The immediate mode only have 5 bits, but it needs to be added to 16-bit number, so, those 5 bits need to be 
        extend to 16-bit to match other number. (otherwise, in case negative number, this causes a a problem)
    */

   if ((x >> (bit_count - 1)) & 1) { // check if negative, a state that have left-most bit = 1
        x != (0xFFFF << bit_count); // 0xFFFF = 1111 1111 1111 1111
   }
   return x;
}

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
                {
                    /* destination register DR */
                    uint16_t r0 = (instr >> 9) & 0x7; // 7 => 0000 111 -> right-most 3 bits
                    /* first operand SR1 */
                    uint16_t r1 = (instr >> 6) & 0x7;
                    /* whether we are in immediate mode */
                    uint16_t imm_flag = (instr >> 5) & 0x1;
                    if (imm_flag) {
                        uint16_t imme = sign_extend((instr & 0x1F), 5); // 0x1F = 0001 1111 -> right-most 5 bits
                        reg[r0] = reg[r1] + imme;
                    } else {
                        uint16_t r2 = instr & 0x7;
                        reg[r0] = reg[r1] + reg[r2];
                    }
                    update_flags(r0);
                }
                break;
            case OP_AND:
                {
                    /* destination register DR */
                    uint16_t r0 = (instr >> 9) & 0x7; // 7 => 0000 111 -> right-most 3 bits
                    /* first operand SR1 */
                    uint16_t r1 = (instr >> 6) & 0x7;
                    /* whether we are in immediate mode */
                    uint16_t imm_flag = (instr >> 5) & 0x1;
                    if (imm_flag) {
                        uint16_t imme = sign_extend((instr & 0x1F), 5); // 0x1F = 0001 1111 -> right-most 5 bits
                        reg[r0] = reg[r1] & imme;
                    } else {
                        uint16_t r2 = instr & 0x7;
                        reg[r0] = reg[r1] & reg[r2];
                    }
                    update_flags(r0);
                }
                break;
            case OP_NOT:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t r1 = (instr >> 6) & 0x7;
                    reg[r0] = ~reg[r1];
                    update_flags(r0);
                }
                break;
            case OP_BR:
                {
                    uint16_t pc_offset = sign_extend((instr & 0x1FF), 9);
                    uint16_t cond_flag = (instr >> 9) & 0x7; /* |n|z|p| */
                    if (cond_flag & reg[R_COND]) {
                        reg[R_PC] += pc_offset;
                    }
                }
                break;
            case OP_JMP: /* also handles RET */
                {
                    /* BaseR */
                    uint16_t r1 = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[r1];
                }
                break;
            case OP_JSR:
                {
                    uint16_t bit11 = (instr >> 11) & 0x1;
                    reg[R_R7] = reg[R_PC];
                    if (bit11 == 0) { /* JSSR */
                        /* BaseR */
                        uint16_t r1 = (instr >> 6) & 0x7;
                        reg[R_PC] = reg[r1];
                    } else { /* JSR */
                        uint16_t pc_offset = sign_extend((instr & 0x7FF), 11);
                        reg[R_PC] += pc_offset;
                    }
                }
                break;
            case OP_LD:
                {
                    /* destination register DR */
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend((instr & 0x1FF), 9);
                    reg[r0] = mem_read(reg[R_PC] + pc_offset);
                    update_flags(r0);
                }
                break;
            case OP_LDI:
                {
                    /* destination register DR */
                    uint16_t r0 = (instr >> 9) & 0x7;
                    /* PCoffset 9 */
                    uint16_t pc_offset = sign_extend((instr & 0x1FF), 9); // 0x1FF = 0001 1111 1111 -> right-most 9 bits
                    /* add pc_offsrt to current PC, look at that memory location to get final address */
                    reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
                    update_flags(r0);
                }
                break;
            case OP_LDR:
                {
                    /* destination register DR */
                    uint16_t r0 = (instr >> 9) & 0x7;
                    /* BaseR */
                    uint16_t r1 = (instr >> 6) & 0x7;
                    uint16_t pc_offset = sign_extend((instr & 0x3F), 6);
                    reg[r0] = mem_read(reg[r1] + pc_offset);
                    update_flags(r0);
                }
                break;
            case OP_LEA:
                {
                    /* destination register DR */
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend((instr & 0x1FF), 9);
                    reg[r0] = reg[R_PC] + pc_offset;
                    update_flags(r0);
                }
                break;
            case OP_ST:
                {
                    /* source register SR */
                    uint16_t r1 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend((instr & 0x1FF), 9);
                    mem_write(reg[R_PC] + pc_offset, reg[r1]);
                }
                break;
            case OP_STI:
                {
                    /* source register SR */
                    uint16_t r1 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend((instr & 0x1FF), 9);
                    mem_write(mem_read(reg[R_PC] + pc_offset), reg[r1]);
                }
                break;
            case OP_STR:
                {
                    /* destination register SR */
                    uint16_t r1 = (instr >> 9) & 0x7;
                    /* BaseR */
                    uint16_t r2 = (instr >> 6) & 0x7;
                    uint16_t pc_offset = sign_extend((instr & 0x3F), 6);
                    mem_write(reg[r2] + pc_offset, reg[r1]);
                }
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