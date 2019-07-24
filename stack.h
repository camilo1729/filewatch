
#ifndef LINKEDSTACK_H_INCLUDED
#define LINKEDSTACK_H_INCLUDED


struct node
{
  char data[255];
  struct node* next;
};

typedef struct node node;

int empty(struct node* head);
struct node* pop(struct node *head, char *element);
struct node* push(struct node* head, char *element);
void init_stack(struct node* head);
#endif
