#ifndef TESTING_H
#define TESTING_H

#include <stdbool.h>

// Assert is defined as a macro
#define ASSERT(expr) if (!(expr)) { return false; }

typedef struct Test {
    char *name;
    bool (*test)();
} Test;

void tests_runner();

void PUT32 ( unsigned int, unsigned int );
#define UART0BASE 0x4000C000

void serial_println(char* string_to_print);
void serial_print(char* string_to_print);
void itoa(int n, char* dst);

#endif