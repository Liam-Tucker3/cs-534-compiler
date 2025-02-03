#include <stdio.h>
#include <string.h>

#define MAX_INSTRUCTIONS 100
#define END "end"

char instructions[MAX_INSTRUCTIONS][10];
int pc = 0;
int top = 0;
int gpr = 0;

void execute(char *instruction) {
    // Add your instruction execution logic here
    printf("Executing: %s\n", instruction);
}

int main() {
    // Example instructions, replace with actual instructions
    strcpy(instructions[0], "LOAD");
    strcpy(instructions[1], "ADD");
    strcpy(instructions[2], "STORE");
    strcpy(instructions[3], END);

    while (strcmp(instructions[pc], END) != 0) {
        int exec = pc;
        pc++;
        execute(instructions[exec]);
    }
    execute(instructions[pc]); // Execute the end statement

    return 0;
}