#ifndef CIRCULAR_QUEUE_HEADER_H
#define CIRCULAR_QUEUE_HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
    const size_t front;        /* index of the first element in the queue */
    const size_t back;         /* index of the element past the last element in the queue */
    const size_t iterator;     /* used internally to iterate the circ_queue in the for_each macro (c89/c99 specific) */
    const size_t length;       /* the number of items contained of the circular queue */
    const size_t capacity;     /* the maximum number of elements the queue can hold */
    const size_t element_size; /* the size of the element (in bytes) that can be stored in the queue */
    uint8_t *const data;       /* pointer to the buffer that holds the queue elements */
} circ_queue_t;

#ifdef __cplusplus
#define CIRC_QUEUE_ELEMENT_CAST(element) (decltype(element))
#else
#define CIRC_QUEUE_ELEMENT_CAST(element) (void *)
#endif

/**
 * @brief macro used to iterate through the circular queue
 * @param circ_queue pointer to the circular queue.
 * @param loop_idx a variable of type size_t used for loop indexing.
 * @param element a pointer type variable that receives address of every current element during iteration
 */
#define circ_queue_for_each(circ_queue, loop_idx, element)                                                                                                      \
    for ((*(size_t *)&circ_queue->iterator) = circ_queue->front, loop_idx = 0;                                                                                  \
         loop_idx < circ_queue->length && (element = CIRC_QUEUE_ELEMENT_CAST(element)(circ_queue->data + ((circ_queue->iterator) * circ_queue->element_size))); \
         loop_idx++,                                                                                                                                            \
                   (*(size_t *)&circ_queue->iterator) = ((circ_queue->iterator + 1) % (circ_queue->capacity)))

#ifdef __cplusplus
extern "C"
{
#endif

    circ_queue_t *circ_queue_create(size_t capacity, size_t element_size);
    void circ_queue_destroy(circ_queue_t *queue);

    int circ_queue_push(circ_queue_t *queue, const void *element, int full_remove_oldest);
    int circ_queue_pop(circ_queue_t *queue, void *element);

    void* circ_queue_front(circ_queue_t *queue);
    void* circ_queue_back(circ_queue_t *queue);

    int circ_queue_is_full(circ_queue_t *queue);
    int circ_queue_is_empty(circ_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif