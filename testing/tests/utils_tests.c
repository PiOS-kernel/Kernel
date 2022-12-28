#include "../testing.h"
#include "../../kernel/utils/utils.h"
#include <stdbool.h>

bool test_memcpy() {
    int vec1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int vec2[10];
    memcpy(vec2, vec1, 10 * sizeof(int));
    for (int i = 0; i < 10; i++)
        ASSERT(vec1[i] == vec2[i]);

    return true;
}

bool test_memset() {
    uint8_t vec[10];
    memset(vec, 1, 10 * sizeof(uint8_t));
    for (int i = 0; i < 10; i++) 
        ASSERT(vec[i] == 1);
        
    return true;
}