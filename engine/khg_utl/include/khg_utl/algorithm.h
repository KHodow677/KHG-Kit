#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  void *first;
  void *second;
} utl_pair;

typedef enum {
  UTL_TYPE_INT,
  UTL_TYPE_CHAR,
  UTL_TYPE_FLOAT,
  UTL_TYPE_DOUBLE,
  UTL_TYPE_SHORT,
  UTL_TYPE_LONG,
  UTL_TYPE_LONG_LONG,
  UTL_TYPE_UNSIGNED_LONG,
  UTL_TYPE_UNSIGNED_CHAR,
  UTL_TYPE_UNSIGNED_LONG_LONG,
  UTL_TYPE_LONG_DOUBLE,
  UTL_TYPE_UNSIGNED_SHORT,
  UTL_TYPE_UNSIGNED_INT
} utl_data_type;

typedef int (*utl_compare_func)(const void *, const void *);
typedef bool (*utl_compare_func_bool)(const void *, const void *);
typedef void (*utl_for_each_op_func)(void *);
typedef void (*utl_accumulate_op_func)(void *, const void *);
typedef bool (*utl_bool_predicate_func)(const void *);
typedef uint32_t (*utl_uniform_random_number_generator)(void);
typedef void (*utl_transform_func)(void *, const void *);
typedef void (*utl_reduce_func)(void *, const void*);
typedef bool (*utl_predicate_func)(const void *);
typedef void (*utl_generator_func)(void *);
typedef bool (*utl_unary_predicate_func)(const void *);

void utl_algorithm_sort(void *base, size_t num, size_t size, utl_compare_func comp);
void utl_algorithm_stable_sort(void *base, size_t num, size_t size, utl_compare_func comp);
void utl_algorithm_copy(const void *source, size_t num, size_t size, void *dest);
void utl_algorithm_for_each(void *base, size_t num, size_t size, utl_for_each_op_func op);
void utl_algorithm_transform(const void *base, size_t num, size_t size, void *result, utl_transform_func op);
void utl_algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, utl_compare_func comp);
void utl_algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, utl_compare_func comp);
void utl_algorithm_fill(void *first, void *last, size_t size, const void *val);
void utl_algorithm_fill_n(void *first, size_t n, size_t size, const void *val);
void utl_algorithm_shuffle(void *base, size_t num, size_t size, utl_uniform_random_number_generator rng);
void utl_algorithm_generate(void *first, void *last, size_t size, utl_generator_func gen);
void utl_algorithm_generate_n(void *first, size_t n, size_t size, utl_generator_func gen);
void utl_algorithm_copy_backward(const void *first, const void *last, size_t size, void *result);
void utl_algorithm_copy_if(const void *first, const void *last, size_t size, void *result, utl_unary_predicate_func pred);
void utl_algorithm_copy_n(const void *first, size_t n, size_t size, void *result);
void utl_algorithm_swap(void *a, void *b, size_t size);
void utl_algorithm_swap_ranges(void *first1, void *first2, size_t num, size_t size);
void utl_algorithm_rotate(void *first, void *middle, void *last, size_t size);
void utl_algorithm_rotate_copy(const void *first, const void *middle, const void *last, size_t size, void *result);
void utl_algorithm_remove_copy(const void *source, size_t num, size_t size, void *result, const void *val, utl_compare_func comp);
void utl_algorithm_replace(void *base, size_t num, size_t size, const void *old_val, const void *new_val, utl_compare_func comp);
void utl_algorithm_replace_if(void *base, size_t num, size_t size, const void *val, utl_bool_predicate_func pred);
void utl_algorithm_iota(void *first, void *last, void *val, size_t size, utl_data_type type);

void *utl_algorithm_find(const void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
size_t utl_algorithm_find_at(const void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
void *utl_algorithm_find_if(const void *base, size_t num, size_t size, utl_bool_predicate_func pred);
void *utl_algorithm_find_if_not(const void *base, size_t num, size_t size, utl_bool_predicate_func pred);
void *utl_algorithm_find_end(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, utl_compare_func comp);
void *utl_algorithm_find_first_of(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, utl_compare_func comp);
void *utl_algorithm_accumulate(const void *base, size_t num, size_t size, void *init, utl_accumulate_op_func op);
void *utl_algorithm_max_element(const void *base, size_t num, size_t size, utl_compare_func comp);
void *utl_algorithm_min_element(const void *base, size_t num, size_t size, utl_compare_func comp);
void *utl_algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
void *utl_algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
void *utl_algorithm_reduce(const void *base, size_t num, size_t size, void *init, utl_reduce_func op);
void *utl_algorithm_partition(void *base, size_t num, size_t size, utl_bool_predicate_func pred);
void *utl_algorithm_adjacent_find(const void *base, size_t num, size_t size, utl_compare_func comp);
void *utl_algorithm_is_sorted_until(const void *base, size_t num, size_t size, utl_compare_func comp);
void *utl_algorithm_remove(void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
void *utl_algorithm_begin(void *base);
void *utl_algorithm_end(void *base, size_t num, size_t size);

bool utl_algorithm_all_of(const void *base, size_t num, size_t size, utl_bool_predicate_func pred);
bool utl_algorithm_any_of(const void *base, size_t num, size_t size, utl_bool_predicate_func pred);
bool utl_algorithm_none_of(const void *base, size_t num, size_t size, utl_bool_predicate_func pred);

size_t utl_algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
size_t utl_algorithm_unique(void *base, size_t num, size_t size, utl_compare_func comp);
size_t utl_algorithm_count(const void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
size_t utl_algorithm_count_if(const void *base, size_t num, size_t size, utl_bool_predicate_func pred);
size_t utl_algorithm_unique_copy(const void *first, size_t num, size_t size, void *result, utl_compare_func comp);
size_t utl_algorithm_remove_copy_if(const void *source, size_t num, size_t size, void *result, utl_bool_predicate_func pred);

utl_pair utl_algorithm_equal_range(const void *base, size_t num, size_t size, const void *val, utl_compare_func comp);
utl_pair utl_algorithm_mismatch(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, utl_compare_func_bool comp);

bool utl_algorithm_next_permutation(void *first, void *last, size_t size, utl_compare_func_bool comp);
bool utl_algorithm_prev_permutation(void *first, void *last, size_t size, utl_compare_func_bool comp);
bool utl_algorithm_equal(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, utl_compare_func comp);
bool utl_algorithm_is_permutation(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, utl_compare_func comp);
bool utl_algorithm_includes(const void *first1, size_t num1, size_t size1, const void *first2, size_t num2, size_t size2, utl_compare_func comp);
bool utl_algorithm_is_sorted(const void *base, size_t num, size_t size, utl_compare_func comp);

const void *utl_algorithm_search(const void *first1, const void *last1, size_t size1, const void *first2, const void *last2, size_t size2, utl_compare_func_bool comp);
const void *utl_algorithm_search_n(const void *first, const void *last1, size_t size, size_t count, const void *val, utl_compare_func_bool comp);

