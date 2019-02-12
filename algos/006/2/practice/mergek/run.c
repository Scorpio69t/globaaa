#include "merge.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    /* 3 -> 5 -> 9 */
    ListNode* one = malloc(sizeof(ListNode));
    ListNode* two = malloc(sizeof(ListNode));
    ListNode* three = malloc(sizeof(ListNode));

    one->val = 3;
    two->val = 5;
    three->val = 9;

    one->next = two;
    two->next = three;
    three->next = NULL;

    /* 1 -> 4 -> 6 */
    ListNode* one1 = malloc(sizeof(ListNode));
    ListNode* two1 = malloc(sizeof(ListNode));
    ListNode* three1 = malloc(sizeof(ListNode));

    one1->val = 1;
    two1->val = 4;
    three1->val = 6;

    one1->next = two1;
    two1->next = three1;
    three1->next = NULL;

    /* 0 -> 7 -> 8 */
    ListNode* one2 = malloc(sizeof(ListNode));
    ListNode* two2 = malloc(sizeof(ListNode));
    ListNode* three2 = malloc(sizeof(ListNode));

    one2->val = 0;
    two2->val = 7;
    three2->val = 8;

    one2->next = two2;
    two2->next = three2;
    three2->next = NULL;

    /* this should create an array of ListNode*s */
    ListNode* array[] = {one, one1, one2};

    /* unsorted */
    printf("unsorted: ");
    for(int i = 0; i < 3; i++) {
      ListNode* head = array[i];
      while(head) {
        printf("%i ", head->val);
	head = head->next;
      }
    }
    printf("\n");

    /* sorted; note this will be destructive i.e. the array will no longer
     * contain the same lls, however, no extra nodes are used. */
    ListNode* n = mergeKLists(array, 3);
    printf("sorted: ");
    print_llist(n);
}
