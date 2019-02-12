#include <stdio.h>
#include "helpers.h"

int main() {
    int nums[] = {2, 7, 11, 15, 13, 26, 3, 5};

    int target = 14;

    int* return_array = twoSum(nums, 8, target);

    print_result(return_array, 2);

    return 0;
}
