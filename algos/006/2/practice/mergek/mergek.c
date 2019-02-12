#include "merge.h"
#include <stdio.h>

/* debugging function */
void print_llist(ListNode* head) {
    while(head) {
      printf("%i ", head->val);
      head = head->next;
    }

    printf("\n");
}

/* place elements into list starting at lists[0] */
void insert_into_list(ListNode* head, ListNode** lhead, ListNode* thead) {
    if(thead->val < head->val) {
      thead->next = head;
      (*lhead) = thead;
      return;
    }

    if(!head->next) {
      head->next = thead;
      thead->next = NULL;
      return;
    }

    while(head) {
      if(thead->val < head->next->val) {
        thead->next = head->next;
        head->next = thead;
        return;
      } else {
        head = head->next;
      }
    }
}

/* O(n) | n = summation of the lengths of the lists */
ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
    for(int i = 1; i < listsSize; i++) {
      ListNode* head = lists[i];
      while(head) {
        ListNode* thead = head;
	head = head->next;
        insert_into_list(lists[0], &lists[0], thead);
      }
    }

    return lists[0];
}
