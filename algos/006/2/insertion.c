#include <stdio.h>

void print_array(int* array, int size);
void insertion(int* array, int size);
void insertion2(int* array, int size);

int main() {

    int array[] = {3, 1, 2, 5, 4};

    printf("Unsorted: ");
    print_array(array, 5);

    printf("Sorted: ");
    insertion2(array, 5);
    print_array(array, 5);

    int array2[] = {5, 4, 3, 2, 1};

    printf("Unsorted: ");
    print_array(array2, 5);

    printf("Sorted: ");
    insertion2(array2, 5);
    print_array(array2, 5);

    int array3[] = {1, 2, 3, 4, 5};

    printf("Unsorted: ");
    print_array(array3, 5);

    printf("Sorted: ");
    insertion2(array3, 5);
    print_array(array3, 5);
}

void print_array(int* array, int size) {
    for(int i = 0; i < size; i++) {
      printf("%i", array[i]);
    }

    printf("\n");
}

/* I think even in the best case this'll run O(n^2) */
void insertion(int* array, int size) {
    for(int i = 0; i < size; i++) {
      for(int k = 0; k <= i; k++) {
        if(array[i] <= array[k]) {
          int temp = array[k];
          array[k] = array[i];
          array[i] = temp;
        }
      }
    }
}

/* I think this'll be omega(n) */
void insertion2(int* array, int size) {
    for(int i = 0; i < size; i++) {
      int ith = i;
      for(int k = i - 1; k >= 0; k--, ith--) {
        if(array[ith] >= array[k]) {
	  break;
	} else {
          int temp = array[ith];
          array[ith] = array[k];
          array[k] = temp;
	}
      }
    }
}
