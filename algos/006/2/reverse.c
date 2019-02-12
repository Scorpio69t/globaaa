#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int i;
    struct node* ptr;
} node;

void print_ll(node* head);
void reverse(node* head, node* pointer, node** new_head);

int main() {

    /* let's manually create a linked list */
    node* head = NULL;

    node* three = malloc(sizeof(node));
    if(!three)
      return -1;

    three->i = 3;
    three->ptr = NULL;

    node* two = malloc(sizeof(node));
    if(!two)
      return -1;

    two->i = 2;
    two->ptr = three;

    node* one = malloc(sizeof(node));
    if(!one)
      return -1;

    one->i = 1;
    one->ptr = two;

    head = one;

    print_ll(head);

    reverse(head, NULL, &head);

    print_ll(head);
}

void print_ll(node* head) {
    while(head) {
      printf("%i ", head->i);
      head = head->ptr;
    }

    printf("\n");
}

/* let's reverse the list recursively */
void reverse(node* head, node* pointer, node** new_head) {
    if(!head)
      return;
    else if(!head->ptr) {
      head->ptr = pointer;
      *new_head = head;
      return;
    } else {
      reverse(head->ptr, head, new_head);
      head->ptr = pointer;
      return;
    }
}
