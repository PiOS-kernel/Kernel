#include <stdint.h>

extern int CREATE_TASK_ID;

void create_task(void (*code)(void *), void* args, uint8_t priority) {
    asm volatile (
        "svc %[syscall_id]\n\t"
        "mov pc, lr\n"
        :
        : [syscall_id] "I" (CREATE_TASK_ID)
        :
    );
}