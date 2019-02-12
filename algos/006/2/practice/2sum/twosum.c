#include <stdlib.h>
#include <stdio.h>

int* twoSum(int* nums, int numsSize, int target) {

    int* return_array = malloc(sizeof(int) * 2);

    for(int i = 0; i < numsSize - 1; i++) {
        for(int j = i;j < numsSize; j++) {
            if (nums[i] + nums[j] == target) {
                *return_array = i;
                *(return_array + 1) = j;
                break;
            }
        }
    }

    return return_array;
}

void print_result(int* result_array, int size) {

    printf("[");

    int i = 0;
    for(; i < size - 1; i++) {
      printf("%i, ", result_array[i]);
    }

    printf("%i]", result_array[i]);
    printf("\n");
}
