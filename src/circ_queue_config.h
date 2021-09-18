
#ifndef CIRCULAR_QUEUE_CONFIG_HEADER_H
#define CIRCULAR_QUEUE_CONFIG_HEADER_H

#define CIRC_QUEUE_MALLOC(size)   malloc(size)
#define CIRC_QUEUE_FREE(ptr)     free(ptr)
#define CIRC_QUEUE_MEMCPY(dest,src,count) memcpy(dest,src,count)

#endif