#include <stdio.h>
#include <stdlib.h>

#define MAX_MEMORY 1024 * 64

// Instructions
#define INSTRUCTION_LDA_IMMEDIATE 0xA9
#define INSTRUCTION_LDA_ZEROPAGE 0xAD
#define INSTRUCTION_LDX_IMMEDIATE 0xA2
#define INSTRUCTION_LDX_ZEROPAGE 0xA6
#define INSTRUCTION_LDY_IMMEDIATE 0xA0
#define INSTRUCTION_LDY_ZEROPAGE 0xA4
#define INSTRUCTION_STA_ZEROPAGE 0x85
#define INSTRUCTION_STA_ABSOLUTE 0x8D
#define INSTRUCTION_STX_ZEROPAGE 0x86
#define INSTRUCTION_STX_ABSOLUTE 0x8E

// ====== Cpu 6502 emulator ========

// Define CPU register types here
typedef unsigned char  BYTE;
typedef unsigned short WORD;

// Define CPU struct here
typedef struct CPU CPU;
// Define Memory struct here
typedef struct Memory Memory;

struct Memory {
    BYTE ram[MAX_MEMORY]; // memory address in bytes
};

struct CPU {
    WORD pc;
    WORD sp;
    BYTE a, x, y;
    BYTE c, z, i, d, b, v, n;
};


// Function prototypes here

Memory* memory_init();
BYTE* memory_operator(Memory* mem, unsigned int address);

void memory_free(Memory *mem);

CPU* cpu_init();
void cpu_reset(CPU *cpu, Memory *mem);
void cpu_execute(CPU *cpu,unsigned int* cycles,  Memory *mem);
BYTE cpu_fetch(CPU *cpu, unsigned int* cycles,  Memory *mem);




void print_memory(Memory *mem) {
    for (int i = 0; i < MAX_MEMORY; i++) {
        printf("%02X ", mem->ram[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
}

int main() {
    Memory *mem = memory_init();
    CPU *cpu = cpu_init();
    unsigned int cycles = 2;
    cpu_reset(cpu, mem);
    mem->ram[0xFFFC] = INSTRUCTION_LDA_ZEROPAGE;
    mem->ram[0xFFFD] = 0x42;  // Load address of instruction into program counter
    mem->ram[0xFFFE] = 0x84; // Load address of instruction into program counter
    cpu_execute(cpu, &cycles, mem);
    print_memory(mem);
    return 0;
}


CPU* cpu_init() {
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    if (!cpu) {
        fprintf(stderr, "Failed to allocate CPU memory\n");
        exit(1);
    }
    cpu->pc = 0;
    cpu->sp = 0xFF;
    cpu->a = 1;
    cpu->x = 1;
    cpu->y = 1;
    cpu->c = 1;
    cpu->z = 1;
    cpu->i = 1;
    cpu->d = 1;
    cpu->b = 1;
    cpu->v = 1;
    cpu->n = 1;
    return cpu;
}


void cpu_reset(CPU *cpu, Memory* mem) {
    cpu->pc = 0xFFFC;
    cpu->sp = 0x0100;
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->c = 0;
    cpu->z = 0;
    cpu->i = 0;
    cpu->d = 0;
    cpu->b = 0;
    cpu->v = 0;
    cpu->n = 0;
    // Load program into memory
    mem = memory_init();
}


void cpu_execute(CPU *cpu, unsigned int* cycles, Memory *mem) {
   while (*cycles > 0) {
     BYTE instruction = cpu_fetch(cpu, cycles, mem);
     printf("Cycles %d\n", *cycles);
     // Execute instruction
     switch (instruction) {
        case INSTRUCTION_LDA_IMMEDIATE: {
            // Load accumulator with value from memory
            BYTE value = *memory_operator(mem, cpu->pc++);
            cpu->a = value;
            cpu->z = (cpu->a == 0);
            cpu->n = (cpu->a & 0x80) >> 7;
            break;
        } case INSTRUCTION_LDA_ZEROPAGE: {
            // Load accumulator with value from zero page
            BYTE address = *memory_operator(mem, cpu->pc++);
            BYTE value = *memory_operator(mem, address);
            cpu->a = value;
            cpu->z = (cpu->a == 0);
            cpu->n = (cpu->a & 0x80) >> 7;
            break;
        } case INSTRUCTION_LDX_ZEROPAGE: {
            // Load index register X with value from zero page
            BYTE address = *memory_operator(mem, cpu->pc++);
            BYTE value = *memory_operator(mem, address);
            cpu->x = value;
            cpu->z = (cpu->x == 0);
            cpu->n = (cpu->x & 0x80) >> 7;
            break;
        }
        //default:
          //  fprintf(stderr, "Unknown instruction: %02X\n", instruction);
            //exit(1);
     }
   }
}


BYTE cpu_fetch(CPU *cpu, unsigned int* cycles, Memory *mem) {
    // Fetch instruction from memory
    BYTE instruction = mem->ram[cpu->pc++];
    // Decrement remaining cycles
    (*cycles)--;
    return instruction;
}


Memory* memory_init() {
    Memory *memory = (Memory*) malloc(sizeof(Memory));
    if (!memory) {
        fprintf(stderr, "Failed to allocate memory memory\n");
        exit(1);
    }
    // Initialize memory with zeros
    for (int i = 0; i < MAX_MEMORY; i++) {
        memory->ram[i] = 0;
    }
    return memory;
}


BYTE* memory_operator(Memory* mem, unsigned int address) {
    // Return a pointer to the specified memory address
    return &mem->ram[address];
}

void memory_free(Memory *mem) {
    // Free memory
    free(mem);
}
