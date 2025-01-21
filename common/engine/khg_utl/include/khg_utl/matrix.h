#pragma once

#include <stdbool.h>

#define UTL_EPSILON 1e-9

typedef struct Matrix {
  unsigned int rows;
  unsigned int cols;
  float *data;
} utl_matrix;

typedef float (*matrix_func)(float); 

utl_matrix *utl_matrix_create(unsigned int rows, unsigned int cols);
utl_matrix *utl_matrix_add(const utl_matrix *matrix1, const utl_matrix *matrix2);
utl_matrix *utl_matrix_subtract(const utl_matrix *matrix1, const utl_matrix *matrix2);
utl_matrix *utl_matrix_multiply(const utl_matrix *matrix1, const utl_matrix *matrix2);
utl_matrix *utl_matrix_create_identity(unsigned int n);
utl_matrix *utl_matrix_get_main_diagonal_as_column(const utl_matrix *matrix);
utl_matrix *utl_matrix_get_main_diagonal_as_row(const utl_matrix *matrix);
utl_matrix *utl_matrix_get_minor_diagonal_as_row(const utl_matrix *matrix);
utl_matrix *utl_matrix_get_minor_diagonal_as_column(const utl_matrix *matrix);
utl_matrix *utl_matrix_transpose(const utl_matrix *matrix);
utl_matrix *utl_matrix_adjugate(const utl_matrix *matrix);
utl_matrix *utl_matrix_inverse(const utl_matrix *matrix);
utl_matrix *utl_matrix_create_submatrix(const utl_matrix *matrix, unsigned int exclude_row, unsigned int exclude_col);
utl_matrix *utl_matrix_power(const utl_matrix *matrix, int power);
utl_matrix *utl_matrix_copy(const utl_matrix *matrix);
utl_matrix *utl_matrix_kronecker_product(const utl_matrix *A, const utl_matrix *B);
utl_matrix *utl_matrix_hankel(const utl_matrix *first_row, const utl_matrix *last_col);
utl_matrix *utl_matrix_toeplitz(const utl_matrix *first_row, const utl_matrix *first_col);
utl_matrix *utl_matrix_from_array(const float *data, unsigned int rows, unsigned int cols);
utl_matrix *utl_matrix_circulant(const utl_matrix *first_row);
utl_matrix *utl_matrix_hilbert(unsigned int n);
utl_matrix *utl_matrix_helmert(unsigned int n, bool full);
utl_matrix *utl_matrix_cofactor(const utl_matrix *matrix);
utl_matrix *utl_matrix_cholesky_decomposition(const utl_matrix *matrix);
utl_matrix *utl_matrix_pascal(unsigned int n);
utl_matrix *utl_matrix_inverse_gauss_jordan(const utl_matrix *matrix);
utl_matrix *utl_matrix_projection(const utl_matrix *matrix);
utl_matrix *utl_matrix_vandermonde(const utl_matrix *matrix, unsigned int n);
utl_matrix *utl_matrix_companion(const utl_matrix *coefficients, unsigned int n);
utl_matrix *utl_matrix_map(const utl_matrix *matrix, matrix_func func);
utl_matrix *utl_matrix_leslie(utl_matrix *f, unsigned int f_size, utl_matrix *s, unsigned int s_size);
utl_matrix *utl_matrix_fiedler(const utl_matrix *matrix);
utl_matrix *utl_matrix_inverse_hilbert(unsigned int n);
utl_matrix *utl_matrix_get_row(const utl_matrix *matrix, unsigned int row);
utl_matrix *utl_matrix_get_col(const utl_matrix *matrix, unsigned int col);
utl_matrix *utl_matrix_block_diag(unsigned int count, ...);
utl_matrix *utl_matrix_random(unsigned int row, unsigned int col, unsigned int start, unsigned int end);
utl_matrix *utl_matrix_walsh(unsigned int n);

void utl_matrix_deallocate(utl_matrix *matrix);
void utl_matrix_print(utl_matrix *matrix);
void utl_matrix_swap_rows(utl_matrix *mat, unsigned int row1, unsigned int row2);
void utl_matrix_swap_cols(utl_matrix *mat, unsigned int col1, unsigned int col2);
void utl_matrix_row_divide(utl_matrix *matrix, unsigned int row, float scalar);
void utl_matrix_row_subtract(utl_matrix *matrix, unsigned int target_row, unsigned int subtract_row, float scalar);

bool utl_matrix_scalar_multiply(utl_matrix *matrix, float scalar);
bool utl_matrix_set(utl_matrix *matrix, unsigned int rows, unsigned int cols, float value);
bool utl_matrix_is_square(const utl_matrix *matrix);
bool utl_matrix_is_equal(const utl_matrix *matrix1, const utl_matrix* matrix2);
bool utl_matrix_is_identity(const utl_matrix *matrix);
bool utl_matrix_is_idempotent(const utl_matrix *matrix);
bool utl_matrix_is_row(const utl_matrix *matrix);
bool utl_matrix_is_column(const utl_matrix *matrix);
bool utl_matrix_is_symmetric(const utl_matrix *matrix);
bool utl_matrix_is_upper_triangular(const utl_matrix *matrix);
bool utl_matrix_is_lower_triangular(const utl_matrix *matrix);
bool utl_matrix_is_skew_symmetric(const utl_matrix *matrix);
bool utl_matrix_is_diagonal(const utl_matrix *matrix);
bool utl_matrix_is_orthogonal(const utl_matrix *matrix);
bool utl_matrix_is_hankel(const utl_matrix *matrix);
bool utl_matrix_is_toeplitz(const utl_matrix *matrix);
bool utl_matrix_lu_decomposition(const utl_matrix *matrix, utl_matrix **l, utl_matrix **u);
bool utl_matrix_qr_decomposition(const utl_matrix *a, utl_matrix **q, utl_matrix **r);
bool utl_matrix_is_positive_definite(const utl_matrix *matrix);
bool utl_matrix_fill(utl_matrix *matrix, float value);
bool utl_matrix_apply_to_row(utl_matrix *matrix, unsigned int row, matrix_func func);
bool utl_matrix_apply_to_col(utl_matrix *matrix, unsigned int col, matrix_func func);
bool utl_matrix_row_addition(utl_matrix *matrix, unsigned int target_row, unsigned int source_row, float scale);
bool utl_matrix_col_addition(utl_matrix *matrix, unsigned int target_col, unsigned int source_col, float scale);
bool utl_matrix_is_sparse(const utl_matrix *matrix);

float utl_matrix_get(const utl_matrix *matrix, unsigned int row, unsigned int col);
float utl_matrix_determinant(const utl_matrix *matrix);
float utl_matrix_trace(const utl_matrix *matrix);
float utl_matrix_frobenius_norm(const utl_matrix *matrix);
float utl_matrix_l1_norm(const utl_matrix *matrix);
float utl_matrix_infinity_norm(const utl_matrix *matrix);
float utl_matrix_min_element(const utl_matrix *matrix);
float utl_matrix_max_element(const utl_matrix *matrix);

float *utl_matrix_to_array(const utl_matrix *matrix);
int utl_matrix_rank(const utl_matrix *matrix);
unsigned int utl_matrix_size(const utl_matrix *matrix);

