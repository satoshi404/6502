#include <stdio.h>
#include <stdlib.h>

// =================================
// ====== Cpu 6502 emulator ========
// =================================

// Define max memory usage for my emulator
#define MAX_MEMORY 1024 * 64 // 65 kb of memory

// ===========================================================================
// Instructions
// ===========================================================================
// Load Instructions
#define INSTRUCTION_LDA_IMMEDIATE       0xA9
#define INSTRUCTION_LDA_ZEROPAGE        0xA5
#define INSTRUCTION_LDX_ZEROPAGE        0xB5
// Jump Instructions
#define INSTRUCTION_JSR                 0xA8 

// ===========================================================================

// Define CPU register types here
typedef unsigned char  DB; // define byte  8 bit  (1 byte)
typedef unsigned short DW; // define word 16 bits (2 bytes)

// Define CPU struct here
typedef struct CPU CPU;

// =========================================================================
struct CPU {
    DW pc, sp;                // program counter and stack pointer
    DB a, x, y;               // registers
    DB c, z, i, d, b, v, n;   // flags
};
// =========================================================================
// Memory functions
DB* memory_operator(DB* memory_ram, unsigned int address);
// =========================================================================
// CPU functions
void cpu_reset(CPU *cpu, DB* memory_ram);
void cpu_execute(CPU *cpu,unsigned int* cycles,  DB* memory_ram);
DB cpu_fetch(CPU *cpu, unsigned int* cycles,  DB* memory_ram);
// =========================================================================

// Print memory statistics
void print_memory(DB* memory_ram) {
    for (int i = 0; i < MAX_MEMORY; i++) {
        printf("%02X ", memory_ram[i]);
        if (i % 16 == 15) printf("\n");
    }
}

int main() {
    DB memory_ram[MAX_MEMORY];
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    if (cpu == NULL) {
        fprintf(stderr, "Failed to allocate CPU memory\n");
        exit(1);
    }
    cpu_reset(cpu, memory_ram); 
    // Cycles to execute the instruction
    unsigned int cycles = 2;
    // ----------------------------------------------------------------
    // Load instruction into memory
    memory_ram[0xFFFC] = INSTRUCTION_LDA_IMMEDIATE;
    memory_ram[0xFFFD] = 0x0;  // Load address of instruction into program counter
    //memory_ram[0xFFFE] = 0x42;  // The value to load into the accumulator
    // ----------------------------------------------------------------
    cpu_execute(cpu, &cycles, memory_ram);
    print_memory(memory_ram);

    // Print CPU flags
    printf("\nCPU Flags\n");
    printf("( C - Carry flag )       = %d\n", cpu->c);
    printf("( Z - Zero flag)         = %d\n", cpu->z);
    printf("( I - Interrupt disable) = %d\n", cpu->i);
    printf("( D - Division mode      = %d\n", cpu->d);
    printf("( B - Break command      = %d\n", cpu->b);
    printf("( V - Overflow flag)     = %d\n", cpu->v);
    printf("( N - Negative flag)     = %d\n", cpu->n);

    // ---- Cleanup ----
    free(cpu);

    return EXIT_SUCCESS;
}

void cpu_reset(CPU *cpu, DB* memory_ram) {
    cpu->pc = 0xFFFC;
    cpu->sp = 0x0100;
    cpu->a = cpu->x = 0;
    cpu->y = cpu->c = 0;
    cpu->z = cpu->i = 0;
    cpu->d = cpu->b = 0;
    cpu->v = cpu->n = 0;
    // Initialize memory with zeros
    for (int i = 0; i < MAX_MEMORY; i++) {
        memory_ram[i] = 0;
    }
}

void cpu_execute(CPU *cpu, unsigned int* cycles, DB* memory_ram) {
   while (*cycles > 0) {
     DB instruction = cpu_fetch(cpu, cycles, memory_ram);
     printf("Cycles %d\n", *cycles);
     // Execute instruction
     switch (instruction) {
        case INSTRUCTION_LDA_IMMEDIATE: {
            // Load accumulator with value from memory
            DB value = *memory_operator(memory_ram, cpu->pc++);
            cpu->a = value;
            cpu->z = (cpu->a == 0);
            cpu->n = (cpu->a & 0x80) >> 7;
            break;
        } case INSTRUCTION_LDA_ZEROPAGE: {
            // Load accumulator with value from zero page
            DB address = *memory_operator(memory_ram, cpu->pc++);
            DB value = *memory_operator(memory_ram, address);
            cpu->a = value;
            cpu->z = (cpu->a == 0);
            cpu->n = (cpu->a & 0x80) >> 7;
            break;
        } case INSTRUCTION_LDX_ZEROPAGE:{
            DB address = *memory_operator(memory_ram, cpu->pc++);
            DB value = *memory_operator(memory_ram, address);
            cpu->x = value;
            cpu->z = (cpu->x == 0);
            cpu->n = (cpu->x & 0x80) >> 7;
            break;
        }
   }
}

DB cpu_fetch(CPU *cpu, unsigned int* cycles, DB* memory_ram) {
    // Fetch instruction from memory
    DB instruction = memory_ram[cpu->pc++];
    // Decrement remaining cycles
    (*cycles)--;
    return instruction;
}

DB* memory_operator(DB* memory_ram, unsigned int address) {
    // Return a pointer to the specified memory address
    return &memory_ram[address];
}
