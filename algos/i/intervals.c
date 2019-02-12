#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
	int i;
	struct node* next;
} node;

int find_intervals(node* ptr, int* array, int size);
int find_in_array(int* array, int size, int value);

/* globals */
int index = 0;
bool interval = false;

int main() {

	/* array of numbers */
	int array[3] = {5, 8, 10};

	/* create linked list */
	node* one = malloc(sizeof(node));
	one->i = 1;

	node* five = malloc(sizeof(node));
	five->i = 5;

	node* eight = malloc(sizeof(node));
	eight->i = 8;

	node* three = malloc(sizeof(node));
	three->i = 3;

	node* ten = malloc(sizeof(node));
	ten->i = 10;

	/* 8 -> 3 -> 5 -> 1 -> 10 */
	eight->next = three;
	three->next = five;
	five->next = one;
	one->next = ten;
	ten->next = NULL;

	printf("%d\n", find_intervals(eight, array, 3));
}

int find_intervals(node* ptr, int* array, int size) {
	if(!ptr && interval)
		return 1;
	if(!ptr)
		return 0;

	int result = find_in_array(array, 3, ptr->i);

	if(result == -1 && interval) {
		interval = false;
		return find_intervals(ptr->next, array, 3) + 1;
	} else if (result == -1 && !interval) {
		return find_intervals(ptr->next, array, 3);
	} else {
		interval = true;
		return find_intervals(ptr->next, array, 3);
	}
}

int find_in_array(int* array, int size, int value) {
	for(int i = index; i < size; i++) {
		if (value == array[i]) {
			return i;
		}
	}
	return -1;
}
