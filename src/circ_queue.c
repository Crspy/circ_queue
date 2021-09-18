#include "circ_queue.h"
#include "circ_queue_config.h"

#ifndef static_assert
#define static_assert(x, msg) int static_assert(int static_assert[(x) ? 1 : -1])
#endif
#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)

typedef struct circ_queue_mutable
{
    size_t front;        /* index the first element in the queue */
    size_t back;         /* index of the element past the last element in the queue */
    size_t iterator;     /* used internally to iterate the circ_queue in the for_each macro (c89/c99 specific) */
    size_t length;       /* the number of items contained of the circular queue */
    size_t capacity;     /* the maximum number of elements the queue can hold */
    size_t element_size; /* the size of the element (in bytes) that can be stored in the queue */
    uint8_t *data;       /* pointer to the buffer that holds the queue elements */
} circ_queue_mutable_t;

VALIDATE_SIZE(circ_queue_mutable_t, sizeof(circ_queue_t));

/**
 * @brief creates a circular queue with the specified capacity and element size
 * @param capacity maximum number of elements that the circular queue can hold
 * @param element_size size of the each element in the qeueue in bytes
 * @return NULL if either capacity, element_size is zero or if couldn't allocate the required memory,
 * otherwise return a pointer to the created circular queue
 */
circ_queue_t *circ_queue_create(size_t capacity, size_t element_size)
{
    circ_queue_mutable_t *queue;

    if (capacity == 0 || element_size == 0)
        return NULL;

    queue = (circ_queue_mutable_t *)CIRC_QUEUE_MALLOC(sizeof(circ_queue_t));
    if (!queue)
        return NULL;

    queue->element_size = element_size;
    queue->capacity = capacity;
    queue->length = 0;
    queue->iterator = 0;
    queue->front = 0;
    queue->back = 0;

    queue->data = (uint8_t *)CIRC_QUEUE_MALLOC(queue->capacity * queue->element_size);

    if (!queue->data)
    {
        CIRC_QUEUE_FREE(queue);
        return NULL;
    }
    return (circ_queue_t *)queue;
}

/**
 * @brief frees the specified circular queue and its resources
 * @param queue pointer to the circular queue.
 */
void circ_queue_destroy(circ_queue_t *queue)
{
    CIRC_QUEUE_FREE(queue->data);
    CIRC_QUEUE_FREE(queue);
}

/**
 * @brief appends an element to the end of the circular queue
 * @param queue pointer to the circular queue.
 * @param element pointer to the element to be appended.
 * @param full_remove_oldest if not 0 the function will remove an element from the front of the queue if it's full
 * @return -1 if there's an overflow , otherwise returns 0
 */
int circ_queue_push(circ_queue_t *queue, const void *element, int full_remove_oldest)
{
    circ_queue_mutable_t *mutable_queue = (circ_queue_mutable_t *)queue;
    if (circ_queue_is_full(queue))
    {
        if (!full_remove_oldest)
        {
            return -1;
        }
        else
        {
            circ_queue_pop(queue, NULL);
        }
    }

    CIRC_QUEUE_MEMCPY(queue->data + (queue->element_size * queue->back), element, queue->element_size);
    mutable_queue->back = (queue->back + 1) % queue->capacity;
    mutable_queue->length++;
    return 0;
}

/**
 * @brief removes an element from the front of the circular queue
 * @param queue pointer to the circular queue.
 * @param element pointer to the variable that will hold the removed element, can be NULL
 * @return -1 if there's an underflow , otherwise returns 0
 */
int circ_queue_pop(circ_queue_t *queue, void *element)
{
    circ_queue_mutable_t *mutable_queue = (circ_queue_mutable_t *)queue;
    if (circ_queue_is_empty(queue))
    {
        return -1;
    }

    if (element)
        CIRC_QUEUE_MEMCPY(element, queue->data + (queue->element_size * queue->front), queue->element_size);
    mutable_queue->front = (queue->front + 1) % queue->capacity;
    mutable_queue->length--;
    return 0;
}

/**
 * @brief returns a pointer to the first/front element in the circular queue
 * @param queue pointer to the circular queue.
 * @return NULL if queue is empty , otherwise returns address to the first element
 */
void *circ_queue_front(circ_queue_t *queue)
{
    if (circ_queue_is_empty(queue))
    {
        return NULL;
    }

    return queue->data + (queue->element_size * queue->front);
}

/**
 * @brief returns a pointer to the last element in the circular queue
 * @param queue pointer to the circular queue.
 * @return NULL if queue is empty , otherwise returns address to the last element
 */
void *circ_queue_back(circ_queue_t *queue)
{
    if (circ_queue_is_empty(queue))
    {
        return NULL;
    }

    return queue->data + (queue->element_size * (((queue->front + queue->length-1) % queue->capacity)));
}

/**
 * @brief checks if queue is full or not
 * @param queue pointer to the circular queue.
 * @return 0 if it's not full otherwise returns non zero value
 */
int circ_queue_is_full(circ_queue_t *queue)
{
    return queue->length == queue->capacity;
}

/**
 * @brief checks if queue is empty or not
 * @param queue pointer to the circular queue.
 * @return 0 if it's not empty otherwise returns non zero value
 */
int circ_queue_is_empty(circ_queue_t *queue)
{
    return queue->length == 0;
}