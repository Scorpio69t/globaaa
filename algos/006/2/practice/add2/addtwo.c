#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void print_ll(ListNode* l) {
    while(l) {
      printf("%i", l->val);
      l = l->next;
    }

    printf("\n");
}

void update(ListNode** result, ListNode** result_head, ListNode** node) {
    if(!(*result)) {
      *result = *node;
      *result_head = *result;
    } else {
      (*result)->next = *node;
      *result = (*result)->next;
    }
}

ListNode* addtwo(ListNode* l1, ListNode* l2) {

    int remainder = 0;

    ListNode* ptr_one = l1;
    ListNode* ptr_two = l2;

    ListNode* result = NULL;
    ListNode* result_head = NULL;

    while(ptr_one || ptr_two) {
      if(ptr_one && ptr_two) {

	/* let's create a new node */
        ListNode* node = malloc(sizeof(ListNode));

	/* update value in new node and keep track of remainder */
	node->val = (ptr_one->val + ptr_two->val + remainder) % 10;
	remainder = (ptr_one->val + ptr_two->val + remainder) / 10;

	/* update list pointers */
	ptr_one = ptr_one->next;
	ptr_two = ptr_two->next;

	/* update return linked list */
	update(&result, &result_head, &node);

      } else if(ptr_one && !ptr_two) {
	ListNode* node = malloc(sizeof(ListNode));

	node->val = (ptr_one->val + remainder) % 10;
	remainder = (ptr_one->val + remainder) / 10;

	ptr_one = ptr_one->next;

	/* update return linked list */
	update(&result, &result_head, &node);

      } else if(!ptr_one && ptr_two) {
	ListNode* node = malloc(sizeof(ListNode));

	node->val = (ptr_two->val + remainder) % 10;
	remainder = (ptr_two->val + remainder) / 10;

	ptr_two = ptr_two->next;

	/* update return linked list */
	update(&result, &result_head, &node);

      } else {
	if(remainder) {
	  ListNode* node = malloc(sizeof(ListNode));

	  node->val = remainder;
	  node->next = NULL;

	  result->next = node;
	  result = result->next;

	  break;
	}
      }
    }

    return result_head;
}
