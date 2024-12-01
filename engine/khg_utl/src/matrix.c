#include "khg_utl/matrix.h"
#include "khg_utl/error_func.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

static bool utl_is_effectively_zero(double value) {
  bool result = fabs(value) < UTL_EPSILON;
  return result;
}

static void utl_generate_walsh_matrix_recursively(double *data, int order, int dim, int start_row, int start_col, int val) {
  if (order == 1) {
    data[start_row * dim + start_col] = val;
    return;
  }
  int half_order = order / 2;
  utl_generate_walsh_matrix_recursively(data, half_order, dim, start_row, start_col, val);
  utl_generate_walsh_matrix_recursively(data, half_order, dim, start_row, start_col + half_order, val);
  utl_generate_walsh_matrix_recursively(data, half_order, dim, start_row + half_order, start_col, val);
  utl_generate_walsh_matrix_recursively(data, half_order, dim, start_row + half_order, start_col + half_order, -val);
}

static inline int utl_min_number(int a, int b) {
  int result = (a < b) ? a : b;
  return result;
}

static double utl_binomial_coefficient(int n, int k) {
  double *c = (double *)malloc(sizeof(double) * (k + 1));
  if (!c) {
    utl_error_func("Memory allocation failed for binomial coefficient calculation", utl_user_defined_data);
    return -1;
  }
  memset(c, 0, sizeof(double) * (k + 1));
  c[0] = 1;
  for (int i = 1; i <= n; i++) {
    for (int j = utl_min_number(i, k); j > 0; j--) {
      c[j] = c[j] + c[j-1];
    }
  }
  double value = c[k];
  free(c);
  return value;
}

static int64_t utl_factorial(int n) {
  int64_t result = 1;
  for (int i = 2; i <= n; ++i) {
    result *= i;
  }
  return result;
}

static int64_t utl_binomial_factorial(int n, int k) {
  if (k > n) {
    utl_error_func("Invalid inputs", utl_user_defined_data);
    return 0;
  }
  if (k == 0 || k == n) {
    return 1;
  }
  int64_t result = utl_factorial(n) / (utl_factorial(k) * utl_factorial(n - k));
  return result;
}


static double utl_dot_product(const double *v1, const double *v2, size_t length) {
  double sum = 0.0;
  for (size_t i = 0; i < length; ++i) {
    sum += v1[i] * v2[i];
  }
  return sum;
}

static void utl_normalize_vector(double *v, size_t length) {
  double norm = sqrt(utl_dot_product(v, v, length));
  for (size_t i = 0; i < length; ++i) {
    v[i] /= norm;
  }
}

static bool utl_matrix_check_diagonal(const utl_matrix *mat, size_t i, size_t j) {
  double res = utl_matrix_get(mat, i, j);
  while (++i < mat->rows && ++j < mat->cols) {
    double next_value = utl_matrix_get(mat, i, j);
    if (next_value != res) {
      return false;
    }
  }
  return true;
}

void utl_subtract_projection(double *u, const double *v, size_t length) {
  double dot_uv = utl_dot_product(u, v, length);
  double dot_vv = utl_dot_product(v, v, length);
  double scale = dot_uv / dot_vv;
  for (size_t i = 0; i < length; ++i) {
    u[i] -= scale * v[i];
  }
}

void utl_matrix_swap_rows(utl_matrix *mat, size_t row1, size_t row2) {
  if (!mat || row1 >= mat->rows || row2 >= mat->rows) {
    utl_error_func("Invalid row indices or matrix is null", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < mat->cols; i++) {
    double temp = mat->data[row1 * mat->cols + i];
    mat->data[row1 * mat->cols + i] = mat->data[row2 * mat->cols + i];
    mat->data[row2 * mat->cols + i] = temp;
  }
}

void utl_matrix_swap_cols(utl_matrix *mat, size_t col1, size_t col2) {
  if (!mat || col1 >= mat->cols || col2 >= mat->cols) {
    utl_error_func("Invalid column indices or matrix is null", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < mat->rows; i++) {
    double temp = mat->data[i * mat->cols + col1];
    mat->data[i * mat->cols + col1] = mat->data[i * mat->cols + col2];
    mat->data[i * mat->cols + col2] = temp;
  }
}

void utl_matrix_row_divide(utl_matrix *matrix, size_t row, double scalar) {
  if (!matrix || row >= matrix->rows) {
    utl_error_func("Invalid row index or matrix is null", utl_user_defined_data);
    return;
  }
  for (size_t col = 0; col < matrix->cols; col++) {
    matrix->data[row * matrix->cols + col] /= scalar;
  }
}

void utl_matrix_row_subtract(utl_matrix *matrix, size_t targetRow, size_t subtractRow, double scalar) {
  if (!matrix || targetRow >= matrix->rows || subtractRow >= matrix->rows) {
    utl_error_func("Invalid row indices or matrix is null", utl_user_defined_data);
    return;
  }
  for (size_t col = 0; col < matrix->cols; col++) {
    matrix->data[targetRow * matrix->cols + col] -= scalar * matrix->data[subtractRow * matrix->cols + col];
  }
}

utl_matrix *utl_matrix_create(size_t rows, size_t cols) {
  if (rows == 0 || cols == 0) {
    utl_error_func("Number of rows or columns is zero", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *matrix = (utl_matrix *)malloc(sizeof(utl_matrix));
  if (!matrix) {
    utl_error_func("Memory allocation failed for matrix object", utl_user_defined_data);
    return NULL;
  }
  size_t total_size = rows * cols * sizeof(double);
  matrix->data = (double *)malloc(total_size);
  if (!matrix->data) {
    utl_error_func("Memory allocation failed for matrix data", utl_user_defined_data);
    free(matrix);
    return NULL;
  }
  matrix->rows = rows;
  matrix->cols = cols;
  for (size_t index = 0; index < rows * cols; index++) {
    matrix->data[index] = 0.0;
  }
  return matrix;
}

utl_matrix *utl_matrix_add(const utl_matrix *matrix1, const utl_matrix *matrix2) {
  if (!matrix1) {
    utl_error_func("Matrix 1 object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix2) {
    utl_error_func("Matrix 2 object is null", utl_user_defined_data);
    return NULL;
  }
  if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
    utl_error_func("The two matrices are not of the same order", utl_user_defined_data);
    return NULL;
  }
  utl_matrix* addition = utl_matrix_create(matrix1->rows, matrix1->cols);
  if (!addition) {
    utl_error_func("Failed to create result matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix1->rows; i++) {
    for (size_t j = 0; j < matrix1->cols; j++) {
      size_t index = i * matrix1->cols + j;
      addition->data[index] = matrix1->data[index] + matrix2->data[index];
    }
  }
  return addition;
}

utl_matrix *utl_matrix_subtract(const utl_matrix *matrix1, const utl_matrix *matrix2) {
  if (!matrix1) {
    utl_error_func("Matrix 1 object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix2) {
    utl_error_func("Matrix 2 object is null", utl_user_defined_data);
    return NULL;
  }
  if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
    utl_error_func("The two matrices are not of the same order", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *subtraction = utl_matrix_create(matrix1->rows, matrix1->cols);
  if (!subtraction) {
    utl_error_func("Failed to create result matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix1->rows; i++) {
    for (size_t j = 0; j < matrix1->cols; j++) {
      size_t index = i * matrix1->cols + j;
      subtraction->data[index] = matrix1->data[index] - matrix2->data[index];
    }
  }
  return subtraction;
}

utl_matrix *utl_matrix_multiply(const utl_matrix *matrix1, const utl_matrix *matrix2) {
  if (!matrix1) {
    utl_error_func("Matrix 1 object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix2) {
    utl_error_func("Matrix 2 object is null", utl_user_defined_data);
    return NULL;
  }
  if (matrix1->cols != matrix2->rows) {
    utl_error_func("Matrix 1's columns do not match matrix 2's rows", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *product = utl_matrix_create(matrix1->rows, matrix2->cols);
  if (!product) {
    utl_error_func("Memory allocation failed for product matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix1->rows; i++) {
    for (size_t j = 0; j < matrix2->cols; j++) {
      double sum = 0.0;
      for (size_t k = 0; k < matrix1->cols; k++) {
        sum += matrix1->data[i * matrix1->cols + k] * matrix2->data[k * matrix2->cols + j];
      }
      product->data[i * product->cols + j] = sum;
    }
  }
  return product;
}

void utl_matrix_deallocate(utl_matrix *matrix) {
  if (!matrix) {
    return;
  }
  free(matrix->data);
  free(matrix);
}

bool utl_matrix_set(utl_matrix *matrix, size_t rows, size_t cols, double value) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (rows >= matrix->rows || cols >= matrix->cols) {
    utl_error_func("Row or column out of bounds", utl_user_defined_data);
    return false;
  }
  size_t index = rows * matrix->cols + cols;
  matrix->data[index] = value;
  return true;
}

void utl_matrix_print(utl_matrix *matrix) {
  if (!matrix) {
    return;
  }
  int max_width = 0;
  for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
    int width = snprintf(NULL, 0, "%.5lf", matrix->data[i]);
    if (width > max_width) {
      max_width = width;
    }
  }
  for (size_t row = 0; row < matrix->rows; row++) {
    printf("| ");
    for (size_t col = 0; col < matrix->cols; col++) {
      size_t index = row * matrix->cols + col;
      printf("%*.*lf ", max_width, 5, matrix->data[index]);
    }
    printf("|\n");
  }
}

double utl_matrix_get(const utl_matrix *matrix, size_t row, size_t col) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    exit(-1);
  }
  if (row >= matrix->rows || col >= matrix->cols) {
    utl_error_func("Row or column out of bounds", utl_user_defined_data);
    exit(-1);
  }
  size_t index = row * matrix->cols + col;
  double value = matrix->data[index];
  return value;
}

bool utl_matrix_scalar_multiply(utl_matrix *matrix, double scalar) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      size_t index = i * matrix->cols + j;
      matrix->data[index] *= scalar;
    }
  }
  return true;
}

bool utl_matrix_is_square(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  bool is_square = matrix->rows == matrix->cols;
  return is_square;
}

utl_matrix *utl_matrix_create_identity(size_t n) {
  utl_matrix *matrix = utl_matrix_create(n, n);
  if (!matrix) {
    utl_error_func("Memory allocation failed for identity matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      double value = (i == j) ? 1.0 : 0.0;
      utl_matrix_set(matrix, i, j, value);
    }
  }
  return matrix;
}

bool utl_matrix_is_equal(const utl_matrix *matrix1, const utl_matrix *matrix2) {
  if (!matrix1) {
    utl_error_func("Matrix 1 object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix2) {
    utl_error_func("Matrix 2 object is null", utl_user_defined_data);
    return false;
  }
  if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
    utl_error_func("Matrices are not of the same dimensions", utl_user_defined_data);
    return false;
  }
  for (size_t index = 0; index < (matrix1->rows * matrix1->cols); index++) {
    if (matrix1->data[index] != matrix2->data[index]) {
      return false;
    }
  }
  return true;
}

bool utl_matrix_is_identity(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      size_t index = i * matrix->cols + j;
      if (i == j) {
        if (matrix->data[index] != 1.0) {
          return false;
        }
      } 
      else if (matrix->data[index] != 0.0) {
        return false;
      }
    }
  }
  return true;
}

bool utl_matrix_is_idempotent(const utl_matrix *matrix) {
  if (!matrix || !utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is null or not square", utl_user_defined_data);
    return false;
  }
  utl_matrix *square = utl_matrix_multiply(matrix, matrix);
  if (!square) {
    utl_error_func("Matrix multiplication failed", utl_user_defined_data);
    return false;
  }
  bool is_idempotent = utl_matrix_is_equal(square, matrix);
  utl_matrix_deallocate(square);
  return is_idempotent;
}

bool utl_matrix_is_row(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  bool is_row = matrix->rows == 1;
  return is_row;
}

bool utl_matrix_is_column(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  bool is_column = matrix->cols == 1;
  return is_column;
}

utl_matrix *utl_matrix_get_main_diagonal_as_column(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *diagonal_matrix = utl_matrix_create(matrix->rows, 1); 
  if (!diagonal_matrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    double value = matrix->data[i * matrix->cols + i];
    utl_matrix_set(diagonal_matrix, i, 0, value);
  }
  return diagonal_matrix;
}

utl_matrix *utl_matrix_get_main_diagonal_as_row(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *diagonal_matrix = utl_matrix_create(1, matrix->cols); 
  if (!diagonal_matrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->cols; i++) {
    double value = matrix->data[i * matrix->cols + i];
    utl_matrix_set(diagonal_matrix, 0, i, value);
  }
  return diagonal_matrix;
}

utl_matrix *utl_matrix_get_minor_diagonal_as_row(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *diagonal_matrix = utl_matrix_create(1, matrix->cols); 
  if (!diagonal_matrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->cols; i++) {
    double value = matrix->data[i * matrix->cols + (matrix->cols - 1 - i)];
    utl_matrix_set(diagonal_matrix, 0, i, value);
  }
  return diagonal_matrix;
}

utl_matrix *utl_matrix_get_minor_diagonal_as_column(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *diagonal_matrix = utl_matrix_create(matrix->rows, 1); 
  if (!diagonal_matrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    double value = matrix->data[i * matrix->cols + (matrix->cols - 1 - i)];
    utl_matrix_set(diagonal_matrix, i, 0, value);
  }
  return diagonal_matrix;
}

utl_matrix *utl_matrix_transpose(const utl_matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *transposed = utl_matrix_create(matrix->cols, matrix->rows);
  if (!transposed) {
    utl_error_func("Memory allocation failed for transposed matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      double value = matrix->data[i * matrix->cols + j];
      utl_matrix_set(transposed, j, i, value);
    }
  }
  return transposed;
}

bool utl_matrix_is_symmetric(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = i + 1; j < matrix->cols; j++) { 
      if (matrix->data[i * matrix->cols + j] != matrix->data[j * matrix->cols + i]) {
        return false;
      }
    }
  }
  return true;
}

bool utl_matrix_is_upper_triangular(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < i; j++) {
      if (matrix->data[i * matrix->cols + j] != 0) {
        return false;
      }
    }
  }
  return true;
}

bool utl_matrix_is_lower_triangular(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = i + 1; j < matrix->cols; j++) {
      if (matrix->data[i * matrix->cols + j] != 0) {
        return false;
      }
    }
  }
  return true;
}

bool utl_matrix_is_skew_symmetric(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    if (matrix->data[i * matrix->cols + i] != 0) {
      return false;
    }
    for (size_t j = i + 1; j < matrix->cols; j++) {
      if (matrix->data[i * matrix->cols + j] != -matrix->data[j * matrix->cols + i]) {
        return false;
      }
    }
  }
  return true;
}

double utl_matrix_determinant(const utl_matrix *matrix) {
  if (matrix->rows != matrix->cols) {
    utl_error_func("Determinant can only be calculated for square matrices", utl_user_defined_data);
    return 0.0;
  }
  if (matrix->rows == 1) {
    return matrix->data[0];
  } 
  else if (matrix->rows == 2) {
    double det = matrix->data[0] * matrix->data[3] - matrix->data[1] * matrix->data[2];
    return det;
  } 
  else {
    double det = 0;
    for (int j1 = 0; j1 < (int)matrix->cols; j1++) {
      utl_matrix *submatrix = utl_matrix_create(matrix->rows - 1, matrix->cols - 1);
      for (int i = 1; i < (int)matrix->rows; i++) {
        int j2 = 0;
        for (int j = 0; j < (int)matrix->cols; j++) {
          if (j == j1) {
            continue;
          }
          utl_matrix_set(submatrix, i - 1, j2++, matrix->data[i * matrix->cols + j]);
        }
      }
      double cofactor = (j1 % 2 == 0 ? 1 : -1) * matrix->data[j1];
      double sub_det = utl_matrix_determinant(submatrix);
      det += cofactor * sub_det;
      utl_matrix_deallocate(submatrix);
    }
    return det;
  }
}

double utl_matrix_trace(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return 0.0;
  }
  if (matrix->rows != matrix->cols) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return 0.0;
  }
  double trace = 0.0;
  for (size_t i = 0; i < matrix->rows; i++) {
    trace += matrix->data[i * matrix->cols + i];
  }
  return trace;
}

utl_matrix *utl_matrix_create_submatrix(const utl_matrix *matrix, size_t exclude_row, size_t exclude_col) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  if (exclude_row >= matrix->rows || exclude_col >= matrix->cols) {
    utl_error_func("Exclude row or exclude col out of bounds", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *submatrix = utl_matrix_create(matrix->rows - 1, matrix->cols - 1);
  if (!submatrix) {
    utl_error_func("Memory allocation failed for submatrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0, sub_i = 0; i < matrix->rows; i++) {
    if (i == exclude_row) {
      continue;
    }
    for (size_t j = 0, sub_j = 0; j < matrix->cols; j++) {
      if (j == exclude_col) {
        continue;
      }
      double value = utl_matrix_get(matrix, i, j);
      utl_matrix_set(submatrix, sub_i, sub_j, value);
      sub_j++;
    }
    sub_i++;
  }
  return submatrix;
}

utl_matrix *utl_matrix_adjugate(const utl_matrix *matrix) {
  if (!matrix || !utl_matrix_is_square(matrix)) {
    utl_error_func("Invalid input matrix (null or not square)", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *cofactor_matrix = utl_matrix_create(matrix->rows, matrix->cols);
  if (!cofactor_matrix) {
    utl_error_func("Memory allocation failed for cofactor matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      utl_matrix *submatrix = utl_matrix_create_submatrix(matrix, i, j);
      double minor = utl_matrix_determinant(submatrix);
      double cofactor = pow(-1, i + j) * minor;
      utl_matrix_set(cofactor_matrix, i, j, cofactor);
      utl_matrix_deallocate(submatrix);
    }
  }
  utl_matrix *adjugate = utl_matrix_transpose(cofactor_matrix);
  utl_matrix_deallocate(cofactor_matrix);
  return adjugate;
}

utl_matrix *utl_matrix_inverse(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Input matrix is not square", utl_user_defined_data);
    return NULL;
  }
  double det = utl_matrix_determinant(matrix);
  if (det == 0) {
    utl_error_func("Matrix is singular and cannot be inverted", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *inverse = utl_matrix_adjugate(matrix);
  if (!inverse) {
    utl_error_func("Failed to calculate adjugate matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < inverse->rows; i++) {
    for (size_t j = 0; j < inverse->cols; j++) {
      inverse->data[i * inverse->cols + j] /= det;
    }
  }
  return inverse;
}

utl_matrix *utl_matrix_copy(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *copy = utl_matrix_create(matrix->rows, matrix->cols);
  if (!copy) {
    utl_error_func("Memory allocation failed for matrix copy", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      copy->data[i * matrix->cols + j] = matrix->data[i * matrix->cols + j];
    }
  }
  return copy;
}

utl_matrix *utl_matrix_power(const utl_matrix *matrix, int power) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  if (power < 0) {
    utl_error_func("Negative power is not supported", utl_user_defined_data);
    return NULL;
  }
  if (power == 0) {
    return utl_matrix_create_identity(matrix->rows);
  }
  utl_matrix *result = utl_matrix_copy(matrix);
  if (power == 1) {
    return result;
  }
  utl_matrix *temp = NULL;
  while (power > 1) {
    if (power % 2 == 0) {
      temp = utl_matrix_multiply(result, result);
      if (!temp) {
        utl_error_func("Matrix multiplication failed", utl_user_defined_data);
        utl_matrix_deallocate(result);
        return NULL;
      }
      utl_matrix_deallocate(result);
      result = temp;
      power /= 2;
    } 
    else {
      temp = utl_matrix_multiply(result, matrix);
      if (!temp) {
        utl_error_func("Matrix multiplication failed", utl_user_defined_data);
        utl_matrix_deallocate(result);
        return NULL;
      }
      utl_matrix_deallocate(result);
      result = temp;
      power--;
    }
  }
  return result;
}

int utl_matrix_rank(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return -1;
  }
  utl_matrix *temp_matrix = utl_matrix_copy(matrix);
  if (!temp_matrix) {
    utl_error_func("Failed to copy matrix", utl_user_defined_data);
    return -1;
  }
  int rank = temp_matrix->cols;
  for (int row = 0; row < rank; row++) {
    if (utl_is_effectively_zero(temp_matrix->data[row * temp_matrix->cols + row])) {
      bool reduce = true;
      for (int i = row + 1; i < (int)temp_matrix->rows; i++) {
        if (!utl_is_effectively_zero(temp_matrix->data[i * temp_matrix->cols + row])) {
          utl_matrix_swap_rows(temp_matrix, row, i);
          reduce = false;
          break;
        }
      }
      if (reduce) {
        rank--;
        for (int i = 0; i < (int)temp_matrix->rows; i++) {
          temp_matrix->data[i * temp_matrix->cols + row] = temp_matrix->data[i * temp_matrix->cols + rank];
        }
        row--;
      }
    } 
    else {
      for (int i = row + 1; i < (int)temp_matrix->rows; i++) {
        double mult = temp_matrix->data[i * temp_matrix->cols + row] / temp_matrix->data[row * temp_matrix->cols + row];
        for (int j = row; j < (int)temp_matrix->cols; j++) {
          temp_matrix->data[i * temp_matrix->cols + j] -= mult * temp_matrix->data[row * temp_matrix->cols + j];
        }
      }
    }
  }
  utl_matrix_deallocate(temp_matrix);
  return rank;
}

bool utl_matrix_is_diagonal(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      if (i != j && !utl_is_effectively_zero(matrix->data[i * matrix->cols + j])) {
        return false;
      }
    }
  }
  return true;
}

bool utl_matrix_is_orthogonal(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  utl_matrix *transpose = utl_matrix_transpose(matrix);
  if (!transpose) {
    utl_error_func("Failed to compute the transpose", utl_user_defined_data);
    return false;
  }
  utl_matrix *product = utl_matrix_multiply(matrix, transpose);
  if (!product) {
    utl_error_func("Failed to multiply matrix by its transpose", utl_user_defined_data);
    utl_matrix_deallocate(transpose);
    return false;
  }
  bool is_orthogonal = utl_matrix_is_identity(product);
  utl_matrix_deallocate(transpose);
  utl_matrix_deallocate(product);
  return is_orthogonal;
}

utl_matrix *utl_matrix_kronecker_product(const utl_matrix *matrix1, const utl_matrix *matrix2) {
  if (!matrix1 || !matrix2) {
    utl_error_func("One or both matrices are null", utl_user_defined_data);
    return NULL;
  }
  size_t m = matrix1->rows, n = matrix1->cols, p = matrix2->rows, q = matrix2->cols;
  utl_matrix *product = utl_matrix_create(m * p, n * q);
  if (!product) {
    utl_error_func("Memory allocation failed for the result matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < m; ++i) {
    for (size_t j = 0; j < n; ++j) {
      for (size_t k = 0; k < p; ++k) {
        for (size_t l = 0; l < q; ++l) {
          double a = utl_matrix_get(matrix1, i, j);
          double b = utl_matrix_get(matrix2, k, l);
          utl_matrix_set(product, i * p + k, j * q + l, a * b);
        }
      }
    }
  }
  return product;
}

utl_matrix *utl_matrix_hankel(const utl_matrix *first_row, const utl_matrix *last_col) {
  if (!first_row || !last_col || first_row->rows != 1 || last_col->cols != 1) {
    utl_error_func("Invalid input matrices (must be a row vector and a column vector)", utl_user_defined_data);
    return NULL;
  }
  size_t n = first_row->cols;
  if (last_col->rows != n) {
    utl_error_func("First row and last column dimensions are incompatible", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *hankel = utl_matrix_create(n, n);
  if (!hankel) {
    utl_error_func("Memory allocation failed for Hankel matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      double value;
      if (i + j < n) {
        value = utl_matrix_get(first_row, 0, i + j);
      } 
      else {
        value = utl_matrix_get(last_col, i + j - n + 1, 0);
      }
      utl_matrix_set(hankel, i, j, value);
    }
  }
  return hankel;
}

bool utl_matrix_is_hankel(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows - 1; i++) {
    for (size_t j = 0; j < matrix->cols - 1; j++) {
      if (i + j >= matrix->rows - 1) {
        continue;
      }
      double value = utl_matrix_get(matrix, i, j);
      if (i + 1 < matrix->rows && j > 0) {
        double next = utl_matrix_get(matrix, i + 1, j - 1);
        if (!utl_is_effectively_zero(value - next)) {
          return false;
        }
      }
    }
  }
  return true;
}

utl_matrix *utl_matrix_toeplitz(const utl_matrix *first_row, const utl_matrix *first_col) {
  if (!first_row || !first_col) {
    utl_error_func("Input matrices are null", utl_user_defined_data);
    return NULL;
  }
  if (first_row->rows != 1) {
    utl_error_func("Matrix must be a row vector", utl_user_defined_data);
    return NULL;
  }
  if (first_col->cols != 1) {
    utl_error_func("Matrix must be a column vector", utl_user_defined_data);
    return NULL;
  }
  size_t rows = first_col->rows;
  size_t cols = first_row->cols;
  utl_matrix *toeplitz_matrix = utl_matrix_create(rows, cols);
  if (!toeplitz_matrix) {
    utl_error_func("Memory allocation failed for Toeplitz matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      double value;
      if (j >= i) {
        value = utl_matrix_get(first_row, 0, j - i);
      } 
      else {
        value = utl_matrix_get(first_col, i - j, 0);
      }
      utl_matrix_set(toeplitz_matrix, i, j, value);
    }
  }
  return toeplitz_matrix;
}

utl_matrix *utl_matrix_from_array(const double *data, size_t rows, size_t cols) {
  if (!data) {
    utl_error_func("Input data is null", utl_user_defined_data);
    return NULL;
  }
  if (rows == 0 || cols == 0) {
    utl_error_func("Rows or columns cannot be zero", utl_user_defined_data);
    return NULL;
  }
  utl_matrix* matrix = utl_matrix_create(rows, cols);
  if (!matrix) {
    utl_error_func("Memory allocation failed for matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      matrix->data[i * cols + j] = data[i * cols + j];
    }
  }
  return matrix;
}

bool utl_matrix_is_toeplitz(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->cols; i++) {
    if (!utl_matrix_check_diagonal(matrix, 0, i)) {
      utl_error_func("Diagonal check failed", utl_user_defined_data);
      return false;
    }
  }
  for (size_t i = 1; i < matrix->rows; i++) {
    if (!utl_matrix_check_diagonal(matrix, i, 0)) {
      utl_error_func("Diagonal check failed", utl_user_defined_data);
      return false;
    }
  }
  return true;
}

utl_matrix *utl_matrix_circulant(const utl_matrix *first_row) {
  if (!first_row || first_row->rows != 1) {
    utl_error_func("Input must be a single-row matrix", utl_user_defined_data);
    return NULL;
  }
  size_t n = first_row->cols;
  utl_matrix *circulant_matrix = utl_matrix_create(n, n);
  if (!circulant_matrix) {
    utl_error_func("Memory allocation failed for circulant matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t row = 0; row < n; ++row) {
    for (size_t col = 0; col < n; ++col) {
      size_t index = (col + row) % n;
      double value = utl_matrix_get(first_row, 0, index);
      utl_matrix_set(circulant_matrix, row, col, value);
    }
  }
  return circulant_matrix;
}

utl_matrix *utl_matrix_hilbert(size_t n) {
  if (n == 0) {
    utl_error_func("Size must be greater than 0", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *hilbert_matrix = utl_matrix_create(n, n);
  if (!hilbert_matrix) {
    utl_error_func("Memory allocation failed for Hilbert matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      double value = 1.0 / ((i + 1) + (j + 1) - 1.0);
      if (!utl_matrix_set(hilbert_matrix, i, j, value)) {
        utl_error_func("Failed to set value", utl_user_defined_data);
        utl_matrix_deallocate(hilbert_matrix);
        return NULL;
      }
    }
  }
  return hilbert_matrix;
}

utl_matrix *utl_matrix_helmert(size_t n, bool full) {
  utl_matrix *helmert_matrix = utl_matrix_create(n, full ? n : n - 1);
  if (!helmert_matrix) {
    utl_error_func("Memory allocation failed for Helmert matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (i == 0) {
        utl_matrix_set(helmert_matrix, i, j, 1.0 / sqrt(n));
      } 
      else if (j < i) {
        double value = 1.0 / sqrt(i * (i + 1.0));
        utl_matrix_set(helmert_matrix, full ? i : i - 1, j, value);
      } 
      else if (j == i) {
        double value = -sqrt((double)i / (i + 1.0));
        utl_matrix_set(helmert_matrix, full ? i : i - 1, j, value);
      }
    }
  }
  return helmert_matrix;
}

utl_matrix *utl_matrix_cofactor(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix must be square", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->rows;
  utl_matrix *cofactor_matrix = utl_matrix_create(n, n);
  if (!cofactor_matrix) {
    utl_error_func("Memory allocation failed for cofactor matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      utl_matrix *submatrix = utl_matrix_create_submatrix(matrix, i, j);
      if (!submatrix) {
        utl_error_func("Failed to create submatrix", utl_user_defined_data);
        utl_matrix_deallocate(cofactor_matrix);
        return NULL;
      }
      double det = utl_matrix_determinant(submatrix);
      utl_matrix_deallocate(submatrix);
      double cofactor = ((i + j) % 2 == 0 ? 1 : -1) * det;
      utl_matrix_set(cofactor_matrix, i, j, cofactor);
    }
  }
  return cofactor_matrix;
}

utl_matrix *utl_matrix_cholesky_decomposition(const utl_matrix *matrix) {
  if (!matrix || matrix->rows != matrix->cols) {
    utl_error_func("Input must be a square matrix", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->rows;
  utl_matrix *chol = utl_matrix_create(n, n);
  if (!chol) {
    utl_error_func("Memory allocation failed for Cholesky matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = i; j < n; j++) {
      double sum = utl_matrix_get(matrix, i, j);
      for (size_t k = 0; k < i; k++) {
        sum -= utl_matrix_get(chol, k, i) * utl_matrix_get(chol, k, j);
      }
      if (i == j) {
        if (sum <= 0.0) {
          utl_error_func("Matrix is not positive definite", utl_user_defined_data);
          utl_matrix_deallocate(chol);
          return NULL;
        }
        utl_matrix_set(chol, i, j, sqrt(sum));
      } 
      else {
        utl_matrix_set(chol, i, j, sum / utl_matrix_get(chol, i, i));
      }
    }
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < i; j++) {
      utl_matrix_set(chol, i, j, 0.0);
    }
  }
  return chol;
}

bool utl_matrix_lu_decomposition(const utl_matrix *matrix, utl_matrix **l, utl_matrix **u) {
  if (!matrix || !utl_matrix_is_square(matrix)) {
    utl_error_func("Matrix must be square for LU decomposition", utl_user_defined_data);
    return false;
  }
  size_t n = matrix->rows;
  *l = utl_matrix_create(n, n);
  *u = utl_matrix_create(n, n);
  if (!(*l) || !(*u)) {
    utl_error_func("Memory allocation failed for L or U", utl_user_defined_data);
    if (*l) utl_matrix_deallocate(*l);
    if (*u) utl_matrix_deallocate(*u);
    return false;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t k = i; k < n; k++) {
      double sum = 0.0;
      for (size_t j = 0; j < i; j++) {
        sum += utl_matrix_get(*l, i, j) * utl_matrix_get(*u, j, k);
      }
      utl_matrix_set(*u, i, k, utl_matrix_get(matrix, i, k) - sum);
    }
    for (size_t k = i; k < n; k++) {
      if (i == k) {
        utl_matrix_set(*l, i, i, 1.0); 
      } 
      else {
        double sum = 0.0;
        for (size_t j = 0; j < i; j++) {
          sum += utl_matrix_get(*l, k, j) * utl_matrix_get(*u, j, i);
        }
        utl_matrix_set(*l, k, i, (utl_matrix_get(matrix, k, i) - sum) / utl_matrix_get(*u, i, i));
      }
    }
  }
  return true;
}

bool utl_matrix_qr_decomposition(const utl_matrix *a, utl_matrix **q, utl_matrix **r) {
  if (!a || a->rows < a->cols) {
    utl_error_func("Matrix must have more rows than columns", utl_user_defined_data);
    return false;
  }
  size_t m = a->rows;
  size_t n = a->cols;
  *q = utl_matrix_create(m, n);
  *r = utl_matrix_create(n, n);
  if (!*q || !*r) {
    utl_error_func("Memory allocation failed for Q and R", utl_user_defined_data);
    return false;
  }
  double *a_col = (double *)malloc(sizeof(double) * m);
  double *q_col = (double *)malloc(sizeof(double) * m);
  if (!a_col || !q_col) {
    utl_error_func("Memory allocation failed for column vectors", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      a_col[j] = utl_matrix_get(a, j, i);
    }
    for (size_t k = 0; k < i; ++k) {
      for (size_t j = 0; j < m; ++j) {
        q_col[j] = utl_matrix_get(*q, j, k);
      }
      utl_subtract_projection(a_col, q_col, m);
    }
    utl_normalize_vector(a_col, m);  
    for (size_t j = 0; j < m; ++j) {
      utl_matrix_set(*q, j, i, a_col[j]);
    }
  }
  for (size_t j = 0; j < n; ++j) {
    for (size_t i = 0; i <= j; ++i) {
      double r_ij = 0.0;
      for (size_t k = 0; k < m; ++k) {
        r_ij += utl_matrix_get(*q, k, i) * utl_matrix_get(a, k, j);
      }
      utl_matrix_set(*r, i, j, r_ij);
    }
  }
  free(a_col);
  free(q_col);
  return true;
}

utl_matrix *utl_matrix_pascal(size_t n) {
  utl_matrix *pascal_matrix = utl_matrix_create(n, n);
  if (!pascal_matrix) {
    utl_error_func("Memory allocation failed for Pascal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j <= i; j++) {
      double value = utl_binomial_coefficient(i + j, i);
      utl_matrix_set(pascal_matrix, i, j, value);
      utl_matrix_set(pascal_matrix, j, i, value);
    }
  }
  return pascal_matrix;
}

double utl_matrix_frobenius_norm(const utl_matrix *matrix) {
  double sum = 0.0;
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      double value = utl_matrix_get(matrix, i, j);
      sum += value * value;
    }
  }
  double frobenius_norm = sqrt(sum);
  return frobenius_norm;
}

double utl_matrix_l1_norm(const utl_matrix *matrix) {
  double max_sum = 0.0;
  for (size_t j = 0; j < matrix->cols; j++) {
    double column_sum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
      column_sum += fabs(utl_matrix_get(matrix, i, j));
    }
    if (column_sum > max_sum) {
      max_sum = column_sum;
    }
  }
  return max_sum;
}

double utl_matrix_infinity_norm(const utl_matrix *matrix) {
  double max_sum = 0.0;
  for (size_t i = 0; i < matrix->rows; i++) {
    double row_sum = 0.0;
    for (size_t j = 0; j < matrix->cols; j++) {
      row_sum += fabs(utl_matrix_get(matrix, i, j));
    }
    if (row_sum > max_sum) {
      max_sum = row_sum;
    }
  }
  return max_sum;
}

utl_matrix *utl_matrix_inverse_gauss_jordan(const utl_matrix *matrix) {
  if (matrix->rows != matrix->cols) {
    utl_error_func("Matrix must be square for inversion", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->rows;
  utl_matrix *augmented = utl_matrix_create(n, 2 * n);
  if (!augmented) {
    utl_error_func("Memory allocation failed for augmented matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      augmented->data[i * 2 * n + j] = matrix->data[i * n + j]; 
      augmented->data[i * 2 * n + j + n] = (i == j) ? 1.0 : 0.0; 
    }
  }
  for (size_t col = 0; col < n; col++) {
    if (augmented->data[col * 2 * n + col] == 0) {
      size_t swapRow = col + 1;
      while (swapRow < n && augmented->data[swapRow * 2 * n + col] == 0) {
        swapRow++;
      }
      if (swapRow == n) {
        utl_error_func("Matrix is singular, cannot invert", utl_user_defined_data);
        utl_matrix_deallocate(augmented);
        return NULL;
      }
      utl_matrix_swap_rows(augmented, col, swapRow);
    }
    utl_matrix_row_divide(augmented, col, augmented->data[col * 2 * n + col]);
    for (size_t row = 0; row < n; row++) {
      if (row != col) {
        utl_matrix_row_subtract(augmented, row, col, augmented->data[row * 2 * n + col]);
      }
    }
  }
  utl_matrix *inverse = utl_matrix_create(n, n);
  if (!inverse) {
    utl_error_func("Memory allocation failed for inverse matrix", utl_user_defined_data);
    utl_matrix_deallocate(augmented);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      inverse->data[i * n + j] = augmented->data[i * 2 * n + j + n];
    }
  }
  utl_matrix_deallocate(augmented);
  return inverse;
}

bool utl_matrix_is_positive_definite(const utl_matrix *matrix) {
  if (!matrix || matrix->rows != matrix->cols) {
    utl_error_func("Matrix must be square to check if it's positive definite", utl_user_defined_data);
    return false;
  }
  if (!utl_matrix_is_symmetric(matrix)) {
    utl_error_func("Matrix is not symmetric", utl_user_defined_data);
    return false;
  }
  utl_matrix *chol = utl_matrix_cholesky_decomposition(matrix);
  bool is_positive_definite = chol != NULL;
  if (is_positive_definite) {
    utl_matrix_deallocate(chol); 
  }
  else {
    utl_error_func("Cholesky decomposition failed", utl_user_defined_data);
    return false;
  }
  return is_positive_definite;
}

utl_matrix *utl_matrix_projection(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *matrix_transpose = utl_matrix_transpose(matrix);
  if (!matrix_transpose) {
    utl_error_func("Transpose calculation failed", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *mta = utl_matrix_multiply(matrix_transpose, matrix);
  if (!mta) {
    utl_error_func("Multiplication matrix^T * matrix failed", utl_user_defined_data);
    utl_matrix_deallocate(matrix_transpose);
    return NULL;
  }
  utl_matrix *mta_inv = utl_matrix_inverse(mta);
  if (!mta_inv) {
    utl_error_func("Inverse calculation failed", utl_user_defined_data);
    utl_matrix_deallocate(matrix_transpose); 
    utl_matrix_deallocate(mta); 
    return NULL;
  }
  utl_matrix *m_mta_inv = utl_matrix_multiply(matrix, mta_inv);
  if (!m_mta_inv) {
    utl_error_func("Multiplication A * (A^T * A)^-1 failed", utl_user_defined_data);
    utl_matrix_deallocate(matrix_transpose);
    utl_matrix_deallocate(mta);
    utl_matrix_deallocate(mta_inv);
    return NULL;
  }
  utl_matrix *projection = utl_matrix_multiply(m_mta_inv, matrix_transpose);
  if (!projection) {
    utl_error_func("Final projection matrix calculation failed", utl_user_defined_data);
  } 
  utl_matrix_deallocate(matrix_transpose);
  utl_matrix_deallocate(mta);
  utl_matrix_deallocate(mta_inv);
  utl_matrix_deallocate(m_mta_inv);
  return projection;
}

utl_matrix *utl_matrix_vandermonde(const utl_matrix *matrix, size_t n) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *vandermonde = utl_matrix_create(n, n);
  if (!vandermonde) {
    utl_error_func("Memory allocation failed for Vandermonde matrix", utl_user_defined_data);
    return NULL; 
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      vandermonde->data[i * n + j] = pow(matrix->data[i], j);
    }
  }
  return vandermonde;
}

utl_matrix *utl_matrix_companion(const utl_matrix *coefficients, size_t degree) {
  if (!coefficients) {
    utl_error_func("Coefficients matrix is null", utl_user_defined_data);
    return NULL;
  }
  size_t n = degree - 1; 
  utl_matrix *companion = utl_matrix_create(n, n);
  if (!companion) {
    utl_error_func("Memory allocation failed for companion matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (j == n - 1) { 
        companion->data[i * n + j] = -coefficients->data[n - 1 - i] / coefficients->data[degree - 1];
      } 
      else if (i == j + 1) { 
        companion->data[i * n + j] = 1;
      } 
      else { 
        companion->data[i * n + j] = 0;
      }
    }
  }
  return companion;
}

bool utl_matrix_fill(utl_matrix *matrix, double value) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false; 
  }
  if (!matrix->data) {
    utl_error_func("Matrix data is null", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      matrix->data[i * matrix->cols + j] = value;
    }
  }
  return true;
}

utl_matrix *utl_matrix_map(const utl_matrix *matrix, matrix_func func) {
  if (!matrix || !func) {
    utl_error_func("Null argument provided", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *result = (utl_matrix *)malloc(sizeof(utl_matrix));
  if (!result) {
    utl_error_func("Memory allocation failed for matrix structure", utl_user_defined_data);
    return NULL;
  }
  result->data = (double*)malloc(matrix->rows * matrix->cols * sizeof(double));
  if (!result->data) {
    utl_error_func("Memory allocation for matrix data failed", utl_user_defined_data);
    free(result); 
    return NULL;
  }
  result->rows = matrix->rows;
  result->cols = matrix->cols;
  for (size_t i = 0; i < matrix->rows; ++i) {
    for (size_t j = 0; j < matrix->cols; ++j) {
      result->data[i * matrix->cols + j] = func(matrix->data[i * matrix->cols + j]);
    }
  }
  return result;
}

double utl_matrix_min_element(const utl_matrix *matrix) {
  if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
    utl_error_func("Invalid matrix provided", utl_user_defined_data);
    return DBL_MAX; 
  }
  double min = DBL_MAX;
  for (size_t i = 0; i < matrix->rows; ++i) {
    for (size_t j = 0; j < matrix->cols; ++j) {
      double value = matrix->data[i * matrix->cols + j];
      if (value < min) {
        min = value;
      }
    }
  }
  return min;
}

double utl_matrix_max_element(const utl_matrix *matrix) {
  if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
    utl_error_func("Invalid matrix provided", utl_user_defined_data);
    return -DBL_MAX; 
  }
  double max = -DBL_MAX;
  for (size_t i = 0; i < matrix->rows; ++i) {
    for (size_t j = 0; j < matrix->cols; ++j) {
      double value = matrix->data[i * matrix->cols + j];
      if (value > max) {
        max = value;
      }
    }
  }
  return max;
}

bool utl_matrix_apply_to_row(utl_matrix *matrix, size_t row, matrix_func func) {
  if (!matrix || !func || row >= matrix->rows) {
    utl_error_func("Invalid arguments, matrix is null or row index out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t j = 0; j < matrix->cols; ++j) {
    matrix->data[row * matrix->cols + j] = func(matrix->data[row * matrix->cols + j]);
  }
  return true;
}

bool utl_matrix_apply_to_col(utl_matrix *matrix, size_t col, matrix_func func) {
  if (!matrix || !func || col >= matrix->cols) {
    utl_error_func("Invalid arguments, matrix is null or column index out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; ++i) {
    matrix->data[i * matrix->cols + col] = func(matrix->data[i * matrix->cols + col]);
  }
  return true;
}

bool utl_matrix_row_addition(utl_matrix *matrix, size_t targetRow, size_t sourceRow, double scale) {
  if (!matrix || targetRow >= matrix->rows || sourceRow >= matrix->rows) {
    utl_error_func("Invalid arguments, matrix is null or row indices out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t j = 0; j < matrix->cols; ++j) {
    matrix->data[targetRow * matrix->cols + j] += scale * matrix->data[sourceRow * matrix->cols + j];
  }
  return true;
}

bool utl_matrix_col_addition(utl_matrix *matrix, size_t targetCol, size_t sourceCol, double scale) {
  if (!matrix || targetCol >= matrix->cols || sourceCol >= matrix->cols) {
    utl_error_func("Invalid arguments, matrix is null or column indices out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; ++i) {
    matrix->data[i * matrix->cols + targetCol] += scale * matrix->data[i * matrix->cols + sourceCol];
  }
  return true;
}

utl_matrix* utl_matrix_leslie(utl_matrix *f, size_t f_size, utl_matrix *s, size_t s_size) {
  if (!f) {
    utl_error_func("Matrix f is null", utl_user_defined_data);
    return NULL;
  }
  if (f_size != s_size + 1) {
    utl_error_func("F size must be one more than S size", utl_user_defined_data);
    return NULL;
  }
  utl_matrix* leslie = utl_matrix_create(f_size, f_size);
  if (!leslie) {
    utl_error_func("Memory allocation failed for Leslie matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < f_size; ++i) {
    utl_matrix_set(leslie, 0, i, f->data[i]);
  }
  for (size_t i = 1; i < f_size; ++i) {
    utl_matrix_set(leslie, i, i - 1, s->data[i - 1]);
  }
  return leslie;
}

utl_matrix *utl_matrix_fiedler(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->cols >= matrix->rows ? matrix->cols : matrix->rows;
  utl_matrix *fiedler = utl_matrix_create(n, n);
  if (!fiedler) {
    utl_error_func("Memory allocation failed for Fiedler matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      double value = fabs(matrix->data[i] - matrix->data[j]);
      utl_matrix_set(fiedler, i, j, value);
    }
  }
  return fiedler;
}

utl_matrix *utl_matrix_inverse_hilbert(size_t n) {
  utl_matrix *inv_h = utl_matrix_create(n, n);
  if (!inv_h) {
    utl_error_func("Memory allocation failed for inverse Hilbert matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      int s = i + j;
      int64_t sign = (s % 2 == 0) ? 1 : -1;
      int64_t numerator = sign * (i + j + 1) * utl_binomial_factorial(n + i, n - j - 1) * utl_binomial_factorial(n + j, n - i - 1) * utl_binomial_factorial(s, i) * utl_binomial_factorial(s, j);
      int64_t denominator = 1;
      double value = (double)numerator / denominator;
      utl_matrix_set(inv_h, i, j, value);
    }
  }
  return inv_h;
}

utl_matrix *utl_matrix_get_row(const utl_matrix *matrix, size_t row) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  else if (row >= matrix->rows) {
    utl_error_func("Row index out of bounds", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *r = utl_matrix_create(1, matrix->cols);
  if (!r) {
    utl_error_func("Memory allocation failed for row", utl_user_defined_data);
    return NULL;
  }
  for (size_t j = 0; j < matrix->cols; j++) {
    utl_matrix_set(r, 0, j, utl_matrix_get(matrix, row, j));
  }
  return r;
}

utl_matrix *utl_matrix_get_col(const utl_matrix *matrix, size_t col) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  else if (col >= matrix->cols) {
    utl_error_func("Column index out of bounds", utl_user_defined_data);
    return NULL;
  }
  utl_matrix *c = utl_matrix_create(matrix->rows, 1);
  if (!c) {
    utl_error_func("Memory allocation failed for column", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    utl_matrix_set(c, i, 0, utl_matrix_get(matrix, i, col));
  }
  return c;
}

double *utl_matrix_to_array(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  size_t size = matrix->rows * matrix->cols * sizeof(double);
  double *data = (double *)malloc(size);
  if (!data) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  memcpy(data, matrix->data, size);
  return data;
}

utl_matrix *utl_matrix_block_diag(size_t count, ...) {
  va_list args;
  size_t total_rows = 0, total_cols = 0;
  va_start(args, count);
  for (size_t i = 0; i < count; ++i) {
    utl_matrix* mat = va_arg(args, utl_matrix*);
    total_rows += mat->rows;
    total_cols += mat->cols;
  }
  va_end(args);
  utl_matrix *result = utl_matrix_create(total_rows, total_cols);
  if (!result) {
    utl_error_func("Memory allocation failed for block diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  size_t current_row = 0, current_col = 0;
  va_start(args, count);
  for (size_t i = 0; i < count; ++i) {
    utl_matrix *mat = va_arg(args, utl_matrix*);
    for (size_t r = 0; r < mat->rows; ++r) {
      memcpy(result->data + (current_row + r) * total_cols + current_col, mat->data + r * mat->cols, mat->cols * sizeof(double));
    }
    current_row += mat->rows;
    current_col += mat->cols;
  }
  va_end(args);
  return result;
}

bool utl_matrix_is_sparse(const utl_matrix *matrix) {
  if (!matrix || !matrix->data) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return false;
  }
  size_t total_elements = matrix->rows * matrix->cols;
  size_t non_zero_count = 0;
  for (size_t i = 0; i < total_elements; ++i) {
    if (matrix->data[i] != 0) {
      ++non_zero_count;
    }
  }
  double non_zero_percentage = (double)non_zero_count / (double)total_elements;
  bool is_sparse = non_zero_percentage < 0.3;
  return is_sparse;
}

size_t utl_matrix_size(const utl_matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return 0;
  }
  size_t size = matrix->rows * matrix->cols;
  return size;
}

utl_matrix *utl_matrix_random(size_t row, size_t col, size_t start, size_t end) {
  utl_matrix *matrix = utl_matrix_create(row, col);
  srand(time(NULL));
  if (!matrix) {
    utl_error_func("Creation of Matrix object failed", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < utl_matrix_size(matrix); i++) {
    matrix->data[i] = (rand() % end) + start;
  }
  return matrix;
}

utl_matrix* utl_matrix_walsh(size_t n) {
  if (n & (n - 1)) {
    utl_error_func("N is not a power of 2", utl_user_defined_data);
    return NULL;
  }
  utl_matrix* walsh_matrix = utl_matrix_create(n, n);
  if (!walsh_matrix) {
    utl_error_func("Memory allocation failed for Walsh matrix", utl_user_defined_data);
    return NULL;
  }
  utl_generate_walsh_matrix_recursively(walsh_matrix->data, n, n, 0, 0, 1);
  return walsh_matrix;
}

