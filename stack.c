#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
//Stack implementation using a linked list


void init_stack(node* head)
{
  head = NULL;
}

struct node* push(node* head, char *element)
{
  struct node* tmp = (node*)malloc(sizeof(node));
  
  if(tmp == NULL)
    {
      exit(EXIT_FAILURE);
    }
  strcpy(tmp->data,element);
  tmp->next = head;
  head = tmp;
  return head;
}


struct node* pop(node *head, char *element)
{
  struct node* tmp = head;
  strcpy(element,head->data);
  head =head->next;
  free(tmp);
  return head;
}

int empty(node* head)
{
  return head==NULL ? 1:0;
}

