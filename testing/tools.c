#include "testing.h"
#include <stdint.h>

void serial_println(char* string_to_print){
    int i = 0;
    uint8_t c;
    i = 0;
    c = string_to_print[i];
    while(c != '\0'){
        PUT32(UART0BASE, c);
        ++i;
        c = string_to_print[i];
    }
    PUT32(UART0BASE, '\n');
    PUT32(UART0BASE, '\r');
}

void serial_print(char* string_to_print){
    int i = 0;
    uint8_t c;
    i = 0;
    c = string_to_print[i];
    while(c != '\0'){
        PUT32(UART0BASE, c);
        ++i;
        c = string_to_print[i];
    }
}

void itoa(int n, char* dst) {
    int i = 0;
    int sign = n;
    if (sign < 0) {
        n = -n;
    }
    do {
        dst[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0) {
        dst[i++] = '-';
    }
    dst[i] = '\0';
    int j = 0;
    for (j = 0; j < i / 2; j++) {
        char temp = dst[j];
        dst[j] = dst[i - j - 1];
        dst[i - j - 1] = temp;
    }
}