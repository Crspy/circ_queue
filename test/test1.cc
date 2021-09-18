#include <gtest/gtest.h>
#include <iostream>
#include "circ_queue.h"
#include <array>

TEST(circ_queue_test, create_and_destroy)
{

  const auto queue_capacity = 1000;
  circ_queue_t *queue = circ_queue_create(queue_capacity, sizeof(int));

  ASSERT_EQ(queue->capacity, queue_capacity);
  ASSERT_EQ(queue->length, 0);

  ASSERT_TRUE(queue);

  circ_queue_destroy(queue);
}

TEST(circ_queue_test, underflow_on_pop_empty)
{

  const auto queue_capacity = 1000;
  circ_queue_t *queue = circ_queue_create(queue_capacity, sizeof(int));

  ASSERT_EQ(queue->length, 0);
  ASSERT_EQ(circ_queue_pop(queue, NULL), -1);
  ASSERT_EQ(circ_queue_pop(queue, NULL), -1);
  ASSERT_EQ(circ_queue_pop(queue, NULL), -1);
  ASSERT_EQ(queue->length, 0);

  int popped_val = 0xDEADBEEF;
  ASSERT_EQ(circ_queue_pop(queue, &popped_val), -1);
  ASSERT_EQ(popped_val, 0xDEADBEEF);

  circ_queue_destroy(queue);
}

TEST(circ_queue_test, overflow_on_push_full)
{

  const auto queue_capacity = 5;
  circ_queue_t *queue = circ_queue_create(queue_capacity, sizeof(int));

  int elem = 55;
  for (size_t i = 0; i < queue_capacity; ++i)
  {
    ASSERT_EQ(queue->length, i);
    ASSERT_EQ(circ_queue_push(queue, &elem, 0), 0);
    elem++;
  }

  // pushing more elements would result in overflow (return -1)
  ASSERT_EQ(circ_queue_push(queue, &elem, 0), -1);
  ASSERT_EQ(queue->length, 5);
  ASSERT_EQ(circ_queue_push(queue, &elem, 0), -1);
  ASSERT_EQ(queue->length, 5);

  circ_queue_destroy(queue);
}

TEST(circ_queue_test, push_pop)
{

  const auto queue_capacity = 5;
  circ_queue_t *queue = circ_queue_create(queue_capacity, sizeof(int));
  ASSERT_EQ(circ_queue_front(queue), nullptr);
  ASSERT_EQ(circ_queue_back(queue), nullptr);

  int elem = 55;
  int popped_elem;

  ASSERT_EQ(circ_queue_push(queue, &elem, 0), 0);
  ASSERT_EQ(queue->length, 1);

  // make sure front and back are the same
  ASSERT_EQ(*(int *)circ_queue_front(queue), 55);
  ASSERT_EQ(*(int *)circ_queue_back(queue), 55);

  ASSERT_EQ(circ_queue_pop(queue, &popped_elem), 0);
  ASSERT_EQ(popped_elem, elem);
  ASSERT_EQ(queue->length, 0);

  // push the numbers between [55,60[ into the queue
  elem = 55;
  for (size_t i = 0; i < queue_capacity; ++i)
  {
    ASSERT_EQ(queue->length, i);
    ASSERT_EQ(circ_queue_push(queue, &elem, 0), 0);
    elem++;
  }
  // make sure front and back matches what we expect
  ASSERT_EQ(*(int *)circ_queue_front(queue), 55);
  ASSERT_EQ(*(int *)circ_queue_back(queue), elem - 1);

  // make sure that try to store more would cause overflow (return -1)
  // also length should stay the same (equal to capacity)
  elem = 55;
  for (size_t i = 0; i < queue_capacity; ++i)
  {
    ASSERT_EQ(queue->length, queue->capacity);
    ASSERT_EQ(circ_queue_push(queue, &elem, 0), -1);
    elem++;
  }

  // pop all the numbers stored in the queue
  // and make sure that they are the same as the numbers we pushed
  elem = 55;
  for (size_t i = 0; i < queue_capacity; ++i)
  {
    ASSERT_EQ(queue->length, queue_capacity - i);
    ASSERT_EQ(circ_queue_pop(queue, &popped_elem), 0);
    ASSERT_EQ(popped_elem, elem);
    elem++;
  }

  ASSERT_EQ(queue->length, 0);
  ASSERT_EQ(queue->capacity, queue_capacity);

  circ_queue_destroy(queue);
}

TEST(circ_queue_test, for_each_test)
{

  const auto queue_capacity = 6;
  circ_queue_t *queue = circ_queue_create(queue_capacity, sizeof(int));

  int arr[] = {55, 57, 59, 66, 68, 71};

  for (auto e : arr)
  {
    ASSERT_EQ(circ_queue_push(queue, &e, 0), 0);
  }

  fprintf(stderr, "front:%d\nback:%d\n", queue->front, queue->back);

  size_t idx;
  int *element;
  circ_queue_for_each(queue, idx, element)
  {
    fprintf(stderr, "idx:%d\n", idx);
    ASSERT_EQ(*element, arr[idx]);
  }

  std::array<int, 6> arr2{55 + 1, 57 + 1, 59 + 1, 66 + 1, 68 + 1, 71 + 1};
  // should overflow
  for (auto e : arr2)
  {
    ASSERT_EQ(circ_queue_push(queue, &e, 0), -1);
  }

  // should remove oldest elements while pushing back!
  for (auto e : arr2)
  {
    ASSERT_EQ(circ_queue_push(queue, &e, 1), 0);
  }

  // make sure current elements are the new ones
  circ_queue_for_each(queue, idx, element)
  {
    fprintf(stderr, "idx:%d\n", idx);
    ASSERT_EQ(*element, arr2[idx]);
  }

  // make sure front and back matches what we expect
  ASSERT_EQ(*(int *)circ_queue_front(queue), arr2[0]);
  ASSERT_EQ(*(int *)circ_queue_back(queue), arr2[arr2.size() - 1]);

  circ_queue_destroy(queue);
}