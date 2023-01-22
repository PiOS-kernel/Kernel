#include <stdint.h>
#include "testing.h"
#include "../kernel/kernel.h"

int main ( void )
{    
    kernel_init();
    tests_runner();
    while(1);
    return 0;
}

void HardFaultISR()
{
    serial_print("HardFault, baby!\n");
	while(1);
}