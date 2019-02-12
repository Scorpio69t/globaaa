typedef struct ListNode {
    int val;
    struct ListNode *next;
} ListNode;

void print_ll(ListNode* l);
void update(ListNode** result, ListNode** result_head, ListNode** node);
ListNode* addtwo(struct ListNode* l1, struct ListNode* l2);

