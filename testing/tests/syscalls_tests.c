#include "../testing.h"
#include "../../kernel/kernel.h"

void mock_task(void *arg)
{
    while(1);
}

bool test_create_task() {
    char* name = "Mario!";
    create_task(mock_task, (void*) name, 0, NULL);
    ASSERT(count_tasks(&READY_QUEUES[0]) == 1);
    TaskTCB* created_task = dequeue(&READY_QUEUES[0]);
    ASSERT(created_task != NULL);

    // On top of the task's stack we should find 8 words set to 0.
    // These are the registers r4-r11
    uint32_t* stack_top = (uint32_t*) created_task->stp;
    for (int i=0; i<8; i++) {
        ASSERT(*stack_top == 0);
        stack_top += 1;
    }

    ASSERT(*stack_top == (uint32_t) name);
    
    // Then we should find registers r1-r3 and r12 set to 0.
    stack_top += 1;
    for (int i=0; i<4; i++) {
        ASSERT(*stack_top == 0);
        stack_top += 1;
    }

    // Then we should find the link register set to 0
    ASSERT(*stack_top == 0);

    // Then we should find the program counter pointing to the task's entry point,
    // with the LSB set to 0
    stack_top += 1;
    ASSERT(*stack_top == ((uint32_t) mock_task & 0xFFFFFFFE));

    // Finally we should find the initial XPSR value
    stack_top += 1;
    ASSERT(*stack_top == 0x01000000);
    
    return true;
}