/*
 * Definition for singly-linked list.
 */

typedef struct ListNode {
    int val;
    struct ListNode* next;
} ListNode;

void print_llist(ListNode* head);
ListNode* mergeKLists(struct ListNode** lists, int listsSize);
