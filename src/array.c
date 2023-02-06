/**
 * MIT License
 * Copyright (c) [2023] The Barfing Fox [Andrea Nardinocchi (andrea@nardinan.it)]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "array.h"
void *f_array_malloc(size_t bucket, size_t node_size) {
  void *result;
  size_t initial_size = ((node_size * bucket) + (sizeof(size_t) * 3));
  if ((result = malloc(initial_size))) {
    memset(result, 0, initial_size);
    result += (sizeof(size_t) * 3);
    d_array_bucket(result) = bucket;
    d_array_size(result) = bucket;
    d_array_node_size(result) = node_size;
  }
  return result;
}
void *f_array_validate_access(void *array, size_t index) {
  void *result = array;
  if (d_array_size(array) <= index) {
    size_t final_nodes = ((index / d_array_bucket(array)) + 1) * d_array_bucket(array),
      final_size = ((d_array_node_size(array) * final_nodes) + (sizeof(size_t) * 3)),
      current_size = ((d_array_node_size(array) * d_array_size(array)) + (sizeof(size_t) * 3));
    if ((result = realloc((result - (sizeof(size_t) * 3)), final_size))) {
      memset((result + current_size), 0, (final_size - current_size));
      result += (sizeof(size_t) * 3);
      d_array_size(result) = final_nodes;
    }
  }
  return result;
}
void f_array_free(void *array) {
  free((array - (sizeof(size_t) * 3)));
}