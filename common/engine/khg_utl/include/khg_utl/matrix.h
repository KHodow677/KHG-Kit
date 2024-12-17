#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

#define UTL_EPSILON 1e-9

typedef struct Matrix {
  size_t rows;
  size_t cols;
  double *data;
} utl_matrix;

typedef double (*matrix_func)(double); 

utl_matrix *utl_matrix_create(size_t rows, size_t cols);
utl_matrix *utl_matrix_add(const utl_matrix *matrix1, const utl_matrix *matrix2);
utl_matrix *utl_matrix_subtract(const utl_matrix *matrix1, const utl_matrix *matrix2);
utl_matrix *utl_matrix_multiply(const utl_matrix *matrix1, const utl_matrix *matrix2);
utl_matrix *utl_matrix_create_identity(size_t n);
utl_matrix *utl_matrix_get_main_diagonal_as_column(const utl_matrix *matrix);
utl_matrix *utl_matrix_get_main_diagonal_as_row(const utl_matrix *matrix);
utl_matrix *utl_matrix_get_minor_diagonal_as_row(const utl_matrix *matrix);
utl_matrix *utl_matrix_get_minor_diagonal_as_column(const utl_matrix *matrix);
utl_matrix *utl_matrix_transpose(const utl_matrix *matrix);
utl_matrix *utl_matrix_adjugate(const utl_matrix *matrix);
utl_matrix *utl_matrix_inverse(const utl_matrix *matrix);
utl_matrix *utl_matrix_create_submatrix(const utl_matrix *matrix, size_t exclude_row, size_t exclude_col);
utl_matrix *utl_matrix_power(const utl_matrix *matrix, int power);
utl_matrix *utl_matrix_copy(const utl_matrix *matrix);
utl_matrix *utl_matrix_kronecker_product(const utl_matrix *A, const utl_matrix *B);
utl_matrix *utl_matrix_hankel(const utl_matrix *first_row, const utl_matrix *last_col);
utl_matrix *utl_matrix_toeplitz(const utl_matrix *first_row, const utl_matrix *first_col);
utl_matrix *utl_matrix_from_array(const double *data, size_t rows, size_t cols);
utl_matrix *utl_matrix_circulant(const utl_matrix *first_row);
utl_matrix *utl_matrix_hilbert(size_t n);
utl_matrix *utl_matrix_helmert(size_t n, bool full);
utl_matrix *utl_matrix_cofactor(const utl_matrix *matrix);
utl_matrix *utl_matrix_cholesky_decomposition(const utl_matrix *matrix);
utl_matrix *utl_matrix_pascal(size_t n);
utl_matrix *utl_matrix_inverse_gauss_jordan(const utl_matrix *matrix);
utl_matrix *utl_matrix_projection(const utl_matrix *matrix);
utl_matrix *utl_matrix_vandermonde(const utl_matrix *matrix, size_t n);
utl_matrix *utl_matrix_companion(const utl_matrix *coefficients, size_t n);
utl_matrix *utl_matrix_map(const utl_matrix *matrix, matrix_func func);
utl_matrix *utl_matrix_leslie(utl_matrix *f, size_t f_size, utl_matrix *s, size_t s_size);
utl_matrix *utl_matrix_fiedler(const utl_matrix *matrix);
utl_matrix *utl_matrix_inverse_hilbert(size_t n);
utl_matrix *utl_matrix_get_row(const utl_matrix *matrix, size_t row);
utl_matrix *utl_matrix_get_col(const utl_matrix *matrix, size_t col);
utl_matrix *utl_matrix_block_diag(size_t count, ...);
utl_matrix *utl_matrix_random(size_t row, size_t col, size_t start, size_t end);
utl_matrix *utl_matrix_walsh(size_t n);

void utl_matrix_deallocate(utl_matrix *matrix);
void utl_matrix_print(utl_matrix *matrix);
void utl_matrix_swap_rows(utl_matrix *mat, size_t row1, size_t row2);
void utl_matrix_swap_cols(utl_matrix *mat, size_t col1, size_t col2);
void utl_matrix_row_divide(utl_matrix *matrix, size_t row, double scalar);
void utl_matrix_row_subtract(utl_matrix *matrix, size_t target_row, size_t subtract_row, double scalar);

bool utl_matrix_scalar_multiply(utl_matrix *matrix, double scalar);
bool utl_matrix_set(utl_matrix *matrix, size_t rows, size_t cols, double value);
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
bool utl_matrix_fill(utl_matrix *matrix, double value);
bool utl_matrix_apply_to_row(utl_matrix *matrix, size_t row, matrix_func func);
bool utl_matrix_apply_to_col(utl_matrix *matrix, size_t col, matrix_func func);
bool utl_matrix_row_addition(utl_matrix *matrix, size_t target_row, size_t source_row, double scale);
bool utl_matrix_col_addition(utl_matrix *matrix, size_t target_col, size_t source_col, double scale);
bool utl_matrix_is_sparse(const utl_matrix *matrix);

double utl_matrix_get(const utl_matrix *matrix, size_t row, size_t col);
double utl_matrix_determinant(const utl_matrix *matrix);
double utl_matrix_trace(const utl_matrix *matrix);
double utl_matrix_frobenius_norm(const utl_matrix *matrix);
double utl_matrix_l1_norm(const utl_matrix *matrix);
double utl_matrix_infinity_norm(const utl_matrix *matrix);
double utl_matrix_min_element(const utl_matrix *matrix);
double utl_matrix_max_element(const utl_matrix *matrix);

double *utl_matrix_to_array(const utl_matrix *matrix);
int utl_matrix_rank(const utl_matrix *matrix);
size_t utl_matrix_size(const utl_matrix *matrix);

