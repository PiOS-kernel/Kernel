#include "../testing.h"
#include "../../kernel/kernel.h"
#include <stdbool.h>

// Heap tests
extern bool test_heap_init();
extern bool test_count_segments();
extern bool test_available_space();
extern bool test_heap_compaction();
extern bool test_heap_compaction1();
extern bool test_allocate_segment();
extern bool test_allocate_segment1();
extern bool test_trim_segment();
extern bool test_heap_alignment();
extern bool test_heap_alignment1();

// Tasks tests
extern bool test_queue_empty();
extern bool test_enqueue();
extern bool test_dequeue();
extern bool test_unlink_task();
extern bool test_stack_push();
extern bool test_stack_start();
extern bool test_stack_end();
extern bool test_schedule();
extern bool test_schedule1();

// Syscalls tests
extern bool test_create_task();

// Utility functions tests
extern bool test_memcpy();
extern bool test_memset();

Test all_tests[] = {
    // Heap tests
    { "test_heap_init", test_heap_init },
    { "test_count_segments", test_count_segments },
    { "test_available_space", test_available_space },
    { "test_heap_compaction", test_heap_compaction },
    { "test_heap_compaction1", test_heap_compaction1 },
    { "test_allocate_segment", test_allocate_segment },
    { "test_allocate_segment1", test_allocate_segment1 },
    { "test_trim_segment", test_trim_segment },
    { "test_heap_alignment", test_heap_alignment },
    { "test_heap_alignment1", test_heap_alignment1 },

    // Tasks tests
    { "test_queue_empty", test_queue_empty },
    { "test_enqueue", test_enqueue },
    { "test_dequeue", test_dequeue },
    { "test_unlink_task", test_unlink_task },
    { "test_stack_push", test_stack_push },
    { "test_stack_start", test_stack_start },
    { "test_stack_end", test_stack_end },
    { "test_schedule", test_schedule },
    { "test_schedule1", test_schedule1 },

    // Syscalls tests
    { "test_create_task", test_create_task },

    // Utility functions tests
    { "test_memcpy", test_memcpy },
    { "test_memset", test_memset },
};

void tests_runner() {
    int tests = sizeof(all_tests) / sizeof(all_tests[0]);
    int passed = 0;
    int failed = 0;
    serial_print("\nRunning tests...\n");
    for (int i = 0; i < tests; i++) {
        // The kernel is initialized before each test to reset the heap
        // and the tasks queues.
        kernel_init();

        serial_print(all_tests[i].name);
        serial_print(" ... ");
        if (all_tests[i].test()) {
            serial_print("[ ok ]\n");
            passed++;
        } else {
            serial_print("[ failed ]\n");
            failed++;
        }
    }

    char test_passed[10];
    char test_failed[10];
    itoa(passed, test_passed);
    itoa(failed, test_failed);
    serial_print("\n\nTests passed: ");
    serial_print(test_passed);
    serial_print("\nTests failed: ");
    serial_println(test_failed);
}
