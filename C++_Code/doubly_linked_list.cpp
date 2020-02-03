#include <stdlib.h>
#include <stdio.h>
#include "doubly_linked_list.h"

DLinkedList* create_dlinkedlist(void) {
    DLinkedList* newList = (DLinkedList*)malloc(sizeof(DLinkedList));
    newList->head = NULL;
    newList->tail = NULL;
    newList->current = NULL;
    newList->size = 0;
    return newList;
}

LLNode* create_llnode(void* data) {
    LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
    newNode->data = data;
    newNode->next = NULL;
    newNode->previous = NULL;
    return newNode;
}

void insertHead(DLinkedList* dLinkedList, void* data){
  LLNode* newNode = create_llnode(data);
  if(dLinkedList->head == NULL){
    dLinkedList->size++;
    dLinkedList->head = newNode;
    dLinkedList->tail = newNode;
  }else{
    dLinkedList->size++;
    newNode->next = dLinkedList->head;
    (dLinkedList->head)->previous = newNode;
    dLinkedList->head = newNode;
  }
}


void insertTail(DLinkedList* dLinkedList, void* data){
  LLNode* newNode = create_llnode(data);
  if(dLinkedList->tail == NULL){
    dLinkedList->size++;
    dLinkedList->head = newNode;
    dLinkedList->tail = newNode;
  }else{
    dLinkedList->size++;
    newNode->previous = dLinkedList->tail;
    (dLinkedList->tail)->next = newNode;
    dLinkedList->tail = newNode;
  }
}

int insertAfter(DLinkedList* dLinkedList, void* newData){
  if(dLinkedList->current == NULL){
    return 0;
  }else{
    LLNode* newNode = create_llnode(newData);
    dLinkedList->size++;
    newNode->next = (dLinkedList->current)->next;
    newNode->previous = dLinkedList->current;
    if((newNode->next) != NULL){
      (newNode->next)->previous = newNode;
    }else{
      dLinkedList->tail = newNode;
    }
    (dLinkedList->current)->next = newNode;
    return 1;
  }
}


int insertBefore(DLinkedList* dLinkedList, void* newData){
  if(dLinkedList->current == NULL){
    return 0;
  }else{
    LLNode* newNode = create_llnode(newData);
    dLinkedList->size++;
    newNode->previous = (dLinkedList->current)->previous;
    newNode->next = dLinkedList->current;
    if((newNode->previous) != NULL){
      (newNode->previous)->next = newNode;
    }else{
      dLinkedList->head = newNode;
    }
    (dLinkedList->current)->previous = newNode;
    return 1;
  }
}



void* deleteBackward(DLinkedList* dLinkedList){
  if(dLinkedList->current == NULL){
    return NULL;
  }
  dLinkedList-> size--;
  LLNode* tempNode = dLinkedList->current;
  if ((dLinkedList->head) == tempNode){ // If Deleting head.
    dLinkedList->head = tempNode->next;
    dLinkedList->current = NULL;
    if (dLinkedList->size == 0){
      dLinkedList->tail = NULL; // If head happened to be tail.
    }else{
      (dLinkedList->head)->previous = NULL;
    }
    free(tempNode->data);
    free(tempNode);
    return NULL;
  }else{
    dLinkedList->current = tempNode->previous;
    (dLinkedList->current)->next = tempNode->next; //Previous's next
    if(dLinkedList->tail == tempNode){ //If deleting tail.
      dLinkedList->tail = dLinkedList->current;
    }else{
      (tempNode->next)->previous = dLinkedList->current; //Next's previous
    }
    free(tempNode->data);
    free(tempNode);
    return (dLinkedList->current)->data;
  }
}



void* deleteForward(DLinkedList* dLinkedList){
  if(dLinkedList->current == NULL){
    return NULL;
  }
  dLinkedList->size--;
  LLNode* tempNode = dLinkedList->current;
  if ((dLinkedList->tail) == tempNode){ // If Deleting tail.
    dLinkedList->tail = tempNode->previous;
    dLinkedList->current = NULL;
    if (dLinkedList->size == 0){
      dLinkedList->head = NULL; // If tail happened to be head.
    }else{
      (dLinkedList->tail)->next = NULL;
    }
    free(tempNode->data);
    free(tempNode);
    return NULL;
  }else{
    dLinkedList->current = tempNode->next;
    (dLinkedList->current)->previous = tempNode->previous; //Next's previous
    if(dLinkedList->head == tempNode){ //If head tail.
      dLinkedList->head = dLinkedList->current;
    }else{
      ((dLinkedList->current)->previous)->next = dLinkedList->current; //Previous's next
    }
    free(tempNode->data);
    free(tempNode);
    return (dLinkedList->current)->data;
  }
}


void destroyList(DLinkedList* dLinkedList){
  if(dLinkedList->head != NULL){
    getHead(dLinkedList);
    while(deleteForward(dLinkedList)){};
  }
  free(dLinkedList);
}



void* getHead(DLinkedList* dLinkedList){
  if (dLinkedList->head == NULL){
    return NULL;
  }else{
    dLinkedList->current = dLinkedList->head;
    return (dLinkedList->current)->data;
  }
}



void* getTail(DLinkedList* dLinkedList){
  if (dLinkedList->tail == NULL){
    return NULL;
  }else{
    dLinkedList->current = dLinkedList->tail;
    return (dLinkedList->current)->data;
  }
}



void* getCurrent(DLinkedList* dLinkedList){
  if(dLinkedList->current == NULL){
    return NULL;
  }else{
    return (dLinkedList->current)->data;
  }
}



void* getNext(DLinkedList* dLinkedList){
  if(dLinkedList->current == NULL){
    return NULL;
  }
  dLinkedList->current = (dLinkedList->current)->next;
  if(dLinkedList->current == NULL){
    return NULL;
  }
  return (dLinkedList->current)->data;
}



void* getPrevious(DLinkedList* dLinkedList){
  if(dLinkedList->current == NULL){
    return NULL;
  }
  dLinkedList->current = (dLinkedList->current)->previous;
  if(dLinkedList->current == NULL){
    return NULL;
  }
  return (dLinkedList->current)->data;
}


int getSize(DLinkedList* dLinkedList){
    return dLinkedList->size;
}