#ifndef DOUBLELINKEDLIST_H
#define DOUBLELINKEDLIST_H
/** @file doubly_linked_list.h */

/********************************************
 * Doubly Linked List library functions *
 * For more details on their functionality, *
 * see the documentation in doubly_linked_list API *
 ********************************************/


/// The structure to store the information of a doubly linked list node
typedef struct llnode_t {
    void* data;
    struct llnode_t* previous;
    struct llnode_t* next;
}LLNode;


/// The structure to store the information of a doubly linked list
typedef struct dlinkedlist_t {
    struct llnode_t* head;
    struct llnode_t* tail;
    struct llnode_t* current;
    int size;
} DLinkedList;


/**
 * create_dlinkedlist
 *
 * Creates a doubly liked list by allocating memory for it on the heap. Initialize the size to 0,
 * as well as head, current, and tail pointer to NULL
 *
 * @return A pointer to an empty dlinkedlist
 */
DLinkedList* create_dlinkedlist(void);


/**
 * InsertHead
 *
 * Insert the data to the head of the doubly linked list.
 * Attention: All insert functions do not make change on the current pointer.
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @param data A void pointer to data the user is adding to the doubly linked list.
 * 
 */
void insertHead(DLinkedList* dLinkedList, void* data);


/**
 * insertTail
 *
 * Insert the data to the tail of the doubly linked list. 
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @param data A void pointer to data the user is adding to the doubly linked list.
 * 
 */
void insertTail(DLinkedList* dLinkedList, void* data);


/**
 * insertAfter
 *
 * Insert the new data to the doubly linked list right after the current pointer
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @param newData A void pointer to the new data that the user want to add after data
 * @return 1 if insert the new data successfully
 *         0 if the current pointer is NULL
 */
int insertAfter(DLinkedList* dLinkedList, void* newData);


/**
 * insertBefore
 *
 * Insert the new data to the doubly linked list right before the current pointer
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @param newData A void pointer to the new data that the user want to add after data
 * @return 1 if insert the new data successfully
 *         0 if the current pointer is NULL
 */
int insertBefore(DLinkedList* dLinkedList, void* newData);


/**
 * deleteBackward
 *
 * Delete the node the current pointer is pointed at, and move the current pointer backwards. 
 * If the current pointer is pointing at list head, new head should be current->next and the current pointer points to NULL.
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return the data of the new current pointer and NULL if the current pointer is NULL
 */
void* deleteBackward(DLinkedList* dLinkedList);


/**
 * deleteForward
 *
 * Delete the node the current pointer is pointed at, and move the current pointer forwards. 
 * If the current pointer is pointing at list tail, new tail should be current->previous and the current pointer points to NULL.
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return the data of the new current pointer and NULL if the current pointer is NULL
 */
void* deleteForward(DLinkedList* dLinkedList);


/**
 * destroyList
 *
 * Destroy the doubly linked list. Everything in the linked list including list structure,
 * nodes and data are all freed from the heap
 *
 * @param dLinkedList A pointer to the doubly linked list
 *
 */
void destroyList(DLinkedList* dLinkedList);


/**
 * getHead
 *
 * Return the data contained in the head of the doubly linked list, and set the list current pointer
 * to head
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return the head data or  NULL if head == NULL
 */
void* getHead(DLinkedList* dLinkedList);


/**
 * getTail
 *
 * Return the data contained in the tail of the doubly linked list, and set the current pointer
 * to tail
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return the tail data or  NULL if tail == NULL
 */
void* getTail(DLinkedList* dLinkedList);


/**
 * getCurrent
 *
 * Return the data the current pointer is pointing at
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return the current data or NULL if current == NULL
 */
void* getCurrent(DLinkedList* dLinkedList);


/**
 * getNext
 *
 * Return the next data the current pointer is pointing at, and move the current pointer to next node
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return the next data or NULL if current == NULL
 */
void* getNext(DLinkedList* dLinkedList);


/**
 * getPrevious
 *
 * Return the previous data the current pointer is pointing at, and move the current pointer to 
 * previous node
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return the previous data or NULL if current == NULL
 */
void* getPrevious(DLinkedList* dLinkedList);


/**
 * getSize
 *
 * Return the size of the doubly linked list
 *
 * @param dLinkedList A pointer to the doubly linked list
 * @return  the size
 */
int getSize(DLinkedList* dLinkedList);
#endif

