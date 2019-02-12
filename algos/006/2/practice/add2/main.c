#include "helpers.h"
#include <stdlib.h>

int main() {

    /* create first linked list */
    ListNode* one = malloc(sizeof(ListNode));
    ListNode* two = malloc(sizeof(ListNode));
    ListNode* three = malloc(sizeof(ListNode));

    one->val = 2;
    two->val = 4;
    three->val = 3;

    one->next = two;
    two->next = three;
    three->next = NULL;

    /* create second linked list */
    ListNode* four = malloc(sizeof(ListNode));
    ListNode* five = malloc(sizeof(ListNode));
    ListNode* six = malloc(sizeof(ListNode));

    four->val = 5;
    five->val = 6;
    six->val = 4;

    four->next = five;
    five->next = six;
    six->next = NULL;

    ListNode* result = addtwo(one, four);

    print_ll(result);
}
