#include "khg_utl/matrix.h"
#include "khg_utl/error_func.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

static bool is_effectively_zero(double value) {
  bool result = fabs(value) < EPSILON;
  return result;
}

static void generateWalshMatrixRecursively(double* data, int order, int dim, int startRow, int startCol, int val) {
  if (order == 1) {
    data[startRow * dim + startCol] = val;
    return;
  }
  int halfOrder = order / 2;
  generateWalshMatrixRecursively(data, halfOrder, dim, startRow, startCol, val);
  generateWalshMatrixRecursively(data, halfOrder, dim, startRow, startCol + halfOrder, val);
  generateWalshMatrixRecursively(data, halfOrder, dim, startRow + halfOrder, startCol, val);
  generateWalshMatrixRecursively(data, halfOrder, dim, startRow + halfOrder, startCol + halfOrder, -val);
}

static inline int min_number(int a, int b) {
  int result = (a < b) ? a : b;
  return result;
}

static double binomial_coefficient(int n, int k) {
  double *C = (double*) malloc(sizeof(double) * (k + 1));
  if (!C) {
    utl_error_func("Memory allocation failed for binomial coefficient calculation", utl_user_defined_data);
    return -1;
  }
  memset(C, 0, sizeof(double) * (k + 1));
  C[0] = 1;
  for (int i = 1; i <= n; i++) {
    for (int j = min_number(i, k); j > 0; j--) {
      C[j] = C[j] + C[j-1];
    }
  }
  double value = C[k];
  free(C);
  return value;
}

static int64_t factorial(int n) {
  int64_t result = 1;
  for (int i = 2; i <= n; ++i) {
    result *= i;
  }
  return result;
}

static int64_t binomial_factorial(int n, int k) {
  if (k > n) {
    utl_error_func("Invalid inputs", utl_user_defined_data);
    return 0;
  }
  if (k == 0 || k == n) {
    return 1;
  }
  int64_t result = factorial(n) / (factorial(k) * factorial(n - k));
  return result;
}


static double dot_product(const double* v1, const double* v2, size_t length) {
  double sum = 0.0;
  for (size_t i = 0; i < length; ++i) {
    sum += v1[i] * v2[i];
  }
  return sum;
}

static void normalize_vector(double* v, size_t length) {
  double norm = sqrt(dot_product(v, v, length));
  for (size_t i = 0; i < length; ++i) {
    v[i] /= norm;
  }
}

void subtract_projection(double* u, const double* v, size_t length) {
  double dot_uv = dot_product(u, v, length);
  double dot_vv = dot_product(v, v, length);
  double scale = dot_uv / dot_vv;
  for (size_t i = 0; i < length; ++i) {
    u[i] -= scale * v[i];
  }
}

void matrix_swap_rows(Matrix* mat, size_t row1, size_t row2) {
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

void matrix_swap_cols(Matrix* mat, size_t col1, size_t col2) {
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

void matrix_row_divide(Matrix* matrix, size_t row, double scalar) {
  if (!matrix || row >= matrix->rows) {
    utl_error_func("Invalid row index or matrix is null", utl_user_defined_data);
    return;
  }
  for (size_t col = 0; col < matrix->cols; col++) {
      matrix->data[row * matrix->cols + col] /= scalar;
  }
}

void matrix_row_subtract(Matrix* matrix, size_t targetRow, size_t subtractRow, double scalar) {
  if (!matrix || targetRow >= matrix->rows || subtractRow >= matrix->rows) {
    utl_error_func("Invalid row indices or matrix is null", utl_user_defined_data);
    return;
  }
  for (size_t col = 0; col < matrix->cols; col++) {
    matrix->data[targetRow * matrix->cols + col] -= scalar * matrix->data[subtractRow * matrix->cols + col];
  }
}

Matrix* matrix_create(size_t rows, size_t cols) {
  if (rows == 0 || cols == 0) {
    utl_error_func("Number of rows or columns is zero", utl_user_defined_data);
    return NULL;
  }
  Matrix* matrix = (Matrix*) malloc(sizeof(Matrix));
  if (!matrix) {
    utl_error_func("Memory allocation failed for matrix object", utl_user_defined_data);
    return NULL;
  }
  size_t totalSize = rows * cols * sizeof(double);
  matrix->data = (double*) malloc(totalSize);
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

Matrix* matrix_add(const Matrix* matrix1, const Matrix* matrix2) {
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
  Matrix* addition = matrix_create(matrix1->rows, matrix1->cols);
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

Matrix* matrix_subtract(const Matrix* matrix1, const Matrix* matrix2) {
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
  Matrix* subtraction = matrix_create(matrix1->rows, matrix1->cols);
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

Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2) {
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
  Matrix* product = matrix_create(matrix1->rows, matrix2->cols);
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

void matrix_deallocate(Matrix* matrix) {
  if (!matrix) {
    return;
  }
  free(matrix->data);
  free(matrix);
}

bool matrix_set(Matrix* matrix, size_t rows, size_t cols, double value) {
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

void matrix_print(Matrix* matrix) {
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

double matrix_get(const Matrix* matrix, size_t row, size_t col) {
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

static bool matrix_check_diagonal(const Matrix* mat, size_t i, size_t j) {
  double res = matrix_get(mat, i, j);
  while (++i < mat->rows && ++j < mat->cols) {
    double next_value = matrix_get(mat, i, j);
    if (next_value != res) {
      return false;
    }
  }
  return true;
}

bool matrix_scalar_multiply(Matrix* matrix, double scalar) {
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

bool matrix_is_square(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  bool isSquare = matrix->rows == matrix->cols;
  return isSquare;
}

Matrix* matrix_create_identity(size_t n) {
  Matrix* matrix = matrix_create(n, n);
  if (!matrix) {
    utl_error_func("Memory allocation failed for identity matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      double value = (i == j) ? 1.0 : 0.0;
      matrix_set(matrix, i, j, value);
    }
  }
  return matrix;
}

bool matrix_is_equal(const Matrix* matrix1, const Matrix* matrix2) {
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

bool matrix_is_identity(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_square(matrix)) {
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

bool matrix_is_idempotent(const Matrix* matrix) {
  if (!matrix || !matrix_is_square(matrix)) {
    utl_error_func("Matrix is null or not square", utl_user_defined_data);
    return false;
  }
  Matrix* square = matrix_multiply(matrix, matrix);
  if (!square) {
    utl_error_func("Matrix multiplication failed", utl_user_defined_data);
    return false;
  }
  bool isIdempotent = matrix_is_equal(square, matrix);
  matrix_deallocate(square);
  return isIdempotent;
}

bool matrix_is_row(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  bool isRow = matrix->rows == 1;
  return isRow;
}

bool matrix_is_columnar(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  bool isColumnar = matrix->cols == 1;
  return isColumnar;
}

Matrix* matrix_get_main_diagonal_as_column(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  Matrix* diagonalMatrix = matrix_create(matrix->rows, 1); 
  if (!diagonalMatrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    double value = matrix->data[i * matrix->cols + i];
    matrix_set(diagonalMatrix, i, 0, value);
  }
  return diagonalMatrix;
}

Matrix* matrix_get_main_diagonal_as_row(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  Matrix* diagonalMatrix = matrix_create(1, matrix->cols); 
  if (!diagonalMatrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->cols; i++) {
    double value = matrix->data[i * matrix->cols + i];
    matrix_set(diagonalMatrix, 0, i, value);
  }
  return diagonalMatrix;
}

Matrix* matrix_get_minor_diagonal_as_row(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  Matrix* diagonalMatrix = matrix_create(1, matrix->cols); 
  if (!diagonalMatrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->cols; i++) {
    double value = matrix->data[i * matrix->cols + (matrix->cols - 1 - i)];
    matrix_set(diagonalMatrix, 0, i, value);
  }
  return diagonalMatrix;
}

Matrix* matrix_get_minor_diagonal_as_column(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  Matrix* diagonalMatrix = matrix_create(matrix->rows, 1); 
  if (!diagonalMatrix) {
    utl_error_func("Memory allocation failed for diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    double value = matrix->data[i * matrix->cols + (matrix->cols - 1 - i)];
    matrix_set(diagonalMatrix, i, 0, value);
  }
  return diagonalMatrix;
}

Matrix* matrix_transpose(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  Matrix* transposed = matrix_create(matrix->cols, matrix->rows);
  if (!transposed) {
    utl_error_func("Memory allocation failed for transposed matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      double value = matrix->data[i * matrix->cols + j];
      matrix_set(transposed, j, i, value);
    }
  }
  return transposed;
}

bool matrix_is_symmetric(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_square(matrix)) {
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

bool matrix_is_upper_triangular(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_square(matrix)) {
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

bool matrix_is_lower_triangular(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_square(matrix)) {
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

bool matrix_is_skew_symmetric(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_square(matrix)) {
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

double matrix_determinant(const Matrix* matrix) {
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
      Matrix* submatrix = matrix_create(matrix->rows - 1, matrix->cols - 1);
      for (int i = 1; i < (int)matrix->rows; i++) {
        int j2 = 0;
        for (int j = 0; j < (int)matrix->cols; j++) {
          if (j == j1) {
            continue;
          }
          matrix_set(submatrix, i - 1, j2++, matrix->data[i * matrix->cols + j]);
        }
      }
      double cofactor = (j1 % 2 == 0 ? 1 : -1) * matrix->data[j1];
      double subDet = matrix_determinant(submatrix);
      det += cofactor * subDet;
      matrix_deallocate(submatrix);
    }
    return det;
  }
}

double matrix_trace(const Matrix* matrix) {
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

Matrix* matrix_create_submatrix(const Matrix* matrix, size_t excludeRow, size_t excludeCol) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  if (excludeRow >= matrix->rows || excludeCol >= matrix->cols) {
    utl_error_func("Exclude row or exclude col out of bounds", utl_user_defined_data);
    return NULL;
  }
  Matrix* submatrix = matrix_create(matrix->rows - 1, matrix->cols - 1);
  if (!submatrix) {
    utl_error_func("Memory allocation failed for submatrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0, sub_i = 0; i < matrix->rows; i++) {
    if (i == excludeRow) {
      continue;
    }
    for (size_t j = 0, sub_j = 0; j < matrix->cols; j++) {
      if (j == excludeCol) {
        continue;
      }
      double value = matrix_get(matrix, i, j);
      matrix_set(submatrix, sub_i, sub_j, value);
      sub_j++;
    }
    sub_i++;
  }
  return submatrix;
}

Matrix* matrix_adjugate(const Matrix* matrix) {
  if (!matrix || !matrix_is_square(matrix)) {
    utl_error_func("Invalid input matrix (null or not square)", utl_user_defined_data);
    return NULL;
  }
  Matrix* cofactorMatrix = matrix_create(matrix->rows, matrix->cols);
  if (!cofactorMatrix) {
    utl_error_func("Memory allocation failed for cofactor matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      Matrix* submatrix = matrix_create_submatrix(matrix, i, j);
      double minor = matrix_determinant(submatrix);
      double cofactor = pow(-1, i + j) * minor;
      matrix_set(cofactorMatrix, i, j, cofactor);
      matrix_deallocate(submatrix);
    }
  }
  Matrix* adjugate = matrix_transpose(cofactorMatrix);
  matrix_deallocate(cofactorMatrix);
  return adjugate;
}

Matrix* matrix_inverse(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Input matrix is not square", utl_user_defined_data);
    return NULL;
  }
  double det = matrix_determinant(matrix);
  if (det == 0) {
    utl_error_func("Matrix is singular and cannot be inverted", utl_user_defined_data);
    return NULL;
  }
  Matrix* inverse = matrix_adjugate(matrix);
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

Matrix* matrix_copy(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  Matrix* copy = matrix_create(matrix->rows, matrix->cols);
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

Matrix* matrix_power(const Matrix* matrix, int power) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return NULL;
  }
  if (power < 0) {
    utl_error_func("Negative power is not supported", utl_user_defined_data);
    return NULL;
  }
  if (power == 0) {
    return matrix_create_identity(matrix->rows);
  }
  Matrix* result = matrix_copy(matrix);
  if (power == 1) {
    return result;
  }
  Matrix* temp = NULL;
  while (power > 1) {
    if (power % 2 == 0) {
      temp = matrix_multiply(result, result);
      if (!temp) {
        utl_error_func("Matrix multiplication failed", utl_user_defined_data);
        matrix_deallocate(result);
        return NULL;
      }
      matrix_deallocate(result);
      result = temp;
      power /= 2;
    } 
    else {
      temp = matrix_multiply(result, matrix);
      if (!temp) {
        utl_error_func("Matrix multiplication failed", utl_user_defined_data);
        matrix_deallocate(result);
        return NULL;
      }
      matrix_deallocate(result);
      result = temp;
      power--;
    }
  }
  return result;
}

int matrix_rank(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return -1;
  }
  Matrix* tempMatrix = matrix_copy(matrix);
  if (!tempMatrix) {
    utl_error_func("Failed to copy matrix", utl_user_defined_data);
    return -1;
  }
  int rank = tempMatrix->cols;
  for (int row = 0; row < rank; row++) {
    if (is_effectively_zero(tempMatrix->data[row * tempMatrix->cols + row])) {
      bool reduce = true;
      for (int i = row + 1; i < (int)tempMatrix->rows; i++) {
        if (!is_effectively_zero(tempMatrix->data[i * tempMatrix->cols + row])) {
          matrix_swap_rows(tempMatrix, row, i);
          reduce = false;
          break;
        }
      }
      if (reduce) {
        rank--;
        for (int i = 0; i < (int)tempMatrix->rows; i++) {
          tempMatrix->data[i * tempMatrix->cols + row] = tempMatrix->data[i * tempMatrix->cols + rank];
        }
        row--;
      }
    } 
    else {
      for (int i = row + 1; i < (int)tempMatrix->rows; i++) {
        double mult = tempMatrix->data[i * tempMatrix->cols + row] / tempMatrix->data[row * tempMatrix->cols + row];
        for (int j = row; j < (int)tempMatrix->cols; j++) {
          tempMatrix->data[i * tempMatrix->cols + j] -= mult * tempMatrix->data[row * tempMatrix->cols + j];
        }
      }
    }
  }
  matrix_deallocate(tempMatrix);
  return rank;
}

bool matrix_is_diagonal(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      if (i != j && !is_effectively_zero(matrix->data[i * matrix->cols + j])) {
        return false;
      }
    }
  }
  return true;
}

bool matrix_is_orthogonal(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix is not square", utl_user_defined_data);
    return false;
  }
  Matrix* transpose = matrix_transpose(matrix);
  if (!transpose) {
    utl_error_func("Failed to compute the transpose");
    return false;
  }
  Matrix* product = matrix_multiply(matrix, transpose);
  if (!product) {
    utl_error_func("Failed to multiply matrix by its transpose", utl_user_defined_data);
    matrix_deallocate(transpose);
    return false;
  }
  bool isOrthogonal = matrix_is_identity(product);
  matrix_deallocate(transpose);
  matrix_deallocate(product);
  return isOrthogonal;
}

Matrix* matrix_kronecker_product(const Matrix* matrix1, const Matrix* matrix2) {
  if (!matrix1 || !matrix2) {
    utl_error_func("One or both matrices are null", utl_user_defined_data);
    return NULL;
  }
  size_t m = matrix1->rows, n = matrix1->cols, p = matrix2->rows, q = matrix2->cols;
  Matrix* product = matrix_create(m * p, n * q);
  if (!product) {
    utl_error_func("Memory allocation failed for the result matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < m; ++i) {
    for (size_t j = 0; j < n; ++j) {
      for (size_t k = 0; k < p; ++k) {
        for (size_t l = 0; l < q; ++l) {
          double a = matrix_get(matrix1, i, j);
          double b = matrix_get(matrix2, k, l);
          matrix_set(product, i * p + k, j * q + l, a * b);
        }
      }
    }
  }
  return product;
}

Matrix* matrix_hankel(const Matrix* firstRow, const Matrix* lastCol) {
  if (!firstRow || !lastCol || firstRow->rows != 1 || lastCol->cols != 1) {
    utl_error_func("Invalid input matrices (must be a row vector and a column vector)", utl_user_defined_data);
    return NULL;
  }
  size_t n = firstRow->cols;
  if (lastCol->rows != n) {
    utl_error_func("First row and last column dimensions are incompatible", utl_user_defined_data);
    return NULL;
  }
  Matrix* hankel = matrix_create(n, n);
  if (!hankel) {
    utl_error_func("Memory allocation failed for Hankel matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      double value;
      if (i + j < n) {
        value = matrix_get(firstRow, 0, i + j);
      } 
      else {
        value = matrix_get(lastCol, i + j - n + 1, 0);
      }
      matrix_set(hankel, i, j, value);
    }
  }
  return hankel;
}

bool matrix_is_hankel(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows - 1; i++) {
    for (size_t j = 0; j < matrix->cols - 1; j++) {
      if (i + j >= matrix->rows - 1) {
        continue;
      }
      double value = matrix_get(matrix, i, j);
      if (i + 1 < matrix->rows && j > 0) {
        double next = matrix_get(matrix, i + 1, j - 1);
        if (!is_effectively_zero(value - next)) {
          return false;
        }
      }
    }
  }
  return true;
}

Matrix* matrix_toeplitz(const Matrix* firstRow, const Matrix* firstCol) {
  if (!firstRow || !firstCol) {
    utl_error_func("Input matrices are null", utl_user_defined_data);
    return NULL;
  }
  if (firstRow->rows != 1) {
    utl_error_func("Matrix must be a row vector", utl_user_defined_data);
    return NULL;
  }
  if (firstCol->cols != 1) {
    utl_error_func("Matrix must be a column vector", utl_user_defined_data);
    return NULL;
  }
  size_t rows = firstCol->rows;
  size_t cols = firstRow->cols;
  Matrix* toeplitzMatrix = matrix_create(rows, cols);
  if (!toeplitzMatrix) {
    utl_error_func("Memory allocation failed for Toeplitz matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      double value;
      if (j >= i) {
        value = matrix_get(firstRow, 0, j - i);
      } 
      else {
        value = matrix_get(firstCol, i - j, 0);
      }
      matrix_set(toeplitzMatrix, i, j, value);
    }
  }
  return toeplitzMatrix;
}

Matrix* matrix_from_array(const double* data, size_t rows, size_t cols) {
  if (!data) {
    utl_error_func("Input data is null", utl_user_defined_data);
    return NULL;
  }
  if (rows == 0 || cols == 0) {
    utl_error_func("Rows or columns cannot be zero", utl_user_defined_data);
    return NULL;
  }
  Matrix* matrix = matrix_create(rows, cols);
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

bool matrix_is_toeplitz(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->cols; i++) {
    if (!matrix_check_diagonal(matrix, 0, i)) {
      utl_error_func("Diagonal check failed", utl_user_defined_data);
      return false;
    }
  }
  for (size_t i = 1; i < matrix->rows; i++) {
    if (!matrix_check_diagonal(matrix, i, 0)) {
      utl_error_func("Diagonal check failed", utl_user_defined_data);
      return false;
    }
  }
  return true;
}

Matrix* matrix_circulant(const Matrix* firstRow) {
  if (!firstRow || firstRow->rows != 1) {
    utl_error_func("Input must be a single-row matrix", utl_user_defined_data);
    return NULL;
  }
  size_t n = firstRow->cols;
  Matrix* circulantMatrix = matrix_create(n, n);
  if (!circulantMatrix) {
    utl_error_func("Memory allocation failed for circulant matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t row = 0; row < n; ++row) {
    for (size_t col = 0; col < n; ++col) {
      size_t index = (col + row) % n;
      double value = matrix_get(firstRow, 0, index);
      matrix_set(circulantMatrix, row, col, value);
    }
  }
  return circulantMatrix;
}

Matrix* matrix_hilbert(size_t n) {
  if (n == 0) {
    utl_error_func("Size must be greater than 0", utl_user_defined_data);
    return NULL;
  }
  Matrix* hilbertMatrix = matrix_create(n, n);
  if (!hilbertMatrix) {
    utl_error_func("Memory allocation failed for Hilbert matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      double value = 1.0 / ((i + 1) + (j + 1) - 1.0);
      if (!matrix_set(hilbertMatrix, i, j, value)) {
        utl_error_func("Failed to set value", utl_user_defined_data);
        matrix_deallocate(hilbertMatrix);
        return NULL;
      }
    }
  }
  return hilbertMatrix;
}

Matrix* matrix_helmert(size_t n, bool full) {
  Matrix* helmertMatrix = matrix_create(n, full ? n : n - 1);
  if (!helmertMatrix) {
    utl_error_func("Memory allocation failed for Helmert matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (i == 0) {
        matrix_set(helmertMatrix, i, j, 1.0 / sqrt(n));
      } 
      else if (j < i) {
        double value = 1.0 / sqrt(i * (i + 1.0));
        matrix_set(helmertMatrix, full ? i : i - 1, j, value);
      } 
      else if (j == i) {
        double value = -sqrt((double)i / (i + 1.0));
        matrix_set(helmertMatrix, full ? i : i - 1, j, value);
      }
    }
  }
  return helmertMatrix;
}

Matrix* matrix_cofactor(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  if (!matrix_is_square(matrix)) {
    utl_error_func("Matrix must be square", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->rows;
  Matrix* cofactorMatrix = matrix_create(n, n);
  if (!cofactorMatrix) {
    utl_error_func("Memory allocation failed for cofactor matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      Matrix* submatrix = matrix_create_submatrix(matrix, i, j);
      if (!submatrix) {
        utl_error_func("Failed to create submatrix", utl_user_defined_data);
        matrix_deallocate(cofactorMatrix);
        return NULL;
      }
      double det = matrix_determinant(submatrix);
      matrix_deallocate(submatrix);
      double cofactor = ((i + j) % 2 == 0 ? 1 : -1) * det;
      matrix_set(cofactorMatrix, i, j, cofactor);
    }
  }
  return cofactorMatrix;
}

Matrix* matrix_cholesky_decomposition(const Matrix* matrix) {
  if (!matrix || matrix->rows != matrix->cols) {
    utl_error_func("Input must be a square matrix", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->rows;
  Matrix* chol = matrix_create(n, n);
  if (!chol) {
    utl_error_func("Memory allocation failed for Cholesky matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = i; j < n; j++) {
      double sum = matrix_get(matrix, i, j);
      for (size_t k = 0; k < i; k++) {
        sum -= matrix_get(chol, k, i) * matrix_get(chol, k, j);
      }
      if (i == j) {
        if (sum <= 0.0) {
          utl_error_func("Matrix is not positive definite", utl_user_defined_data);
          matrix_deallocate(chol);
          return NULL;
        }
        matrix_set(chol, i, j, sqrt(sum));
      } 
      else {
        matrix_set(chol, i, j, sum / matrix_get(chol, i, i));
      }
    }
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < i; j++) {
      matrix_set(chol, i, j, 0.0);
    }
  }
  return chol;
}

bool matrix_lu_decomposition(const Matrix* matrix, Matrix** L, Matrix** U) {
    MATRIX_LOG("[matrix_lu_decomposition] Entering function");

    if (!matrix || !matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_lu_decomposition] Error: Matrix must be square for LU decomposition.");
        return false;
    }

    size_t n = matrix->rows;
    *L = matrix_create(n, n);
    *U = matrix_create(n, n);

    if (!(*L) || !(*U)) {
        MATRIX_LOG("[matrix_lu_decomposition] Error: Memory allocation failed for L or U.");
        if (*L) matrix_deallocate(*L);
        if (*U) matrix_deallocate(*U);
        return false;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t k = i; k < n; k++) {
            double sum = 0.0;
            for (size_t j = 0; j < i; j++) {
                sum += matrix_get(*L, i, j) * matrix_get(*U, j, k);
            }
            matrix_set(*U, i, k, matrix_get(matrix, i, k) - sum);
            MATRIX_LOG("[matrix_lu_decomposition] Set U(%zu, %zu) = %lf", i, k, matrix_get(matrix, i, k) - sum);
        }

        // Lower Triangular
        for (size_t k = i; k < n; k++) {
            if (i == k) {
                matrix_set(*L, i, i, 1.0); 
                MATRIX_LOG("[matrix_lu_decomposition] Set L(%zu, %zu) = 1.0", i, i);
            } 
            else {
                double sum = 0.0;
                for (size_t j = 0; j < i; j++) {
                    sum += matrix_get(*L, k, j) * matrix_get(*U, j, i);
                }
                matrix_set(*L, k, i, (matrix_get(matrix, k, i) - sum) / matrix_get(*U, i, i));
                MATRIX_LOG("[matrix_lu_decomposition] Set L(%zu, %zu) = %lf", k, i, (matrix_get(matrix, k, i) - sum) / matrix_get(*U, i, i));
            }
        }
    }

    MATRIX_LOG("[matrix_lu_decomposition] Exiting function.");
    return true;
}

/**
 * @brief Performs QR decomposition of a matrix.
 *
 * This function decomposes a given matrix into two matrices: 
 * an orthogonal matrix `Q` and an upper triangular matrix `R`, 
 * such that the original matrix `A` can be represented as `A = Q * R`.
 * Note: The input matrix must have more rows than columns (m >= n).
 *
 * @param A The input matrix to decompose.
 * @param Q Pointer to a pointer that will store the orthogonal matrix.
 * @param R Pointer to a pointer that will store the upper triangular matrix.
 * 
 * @return `true` if the decomposition is successful, `false` otherwise.
 */
bool matrix_qr_decomposition(const Matrix* A, Matrix** Q, Matrix** R) {
    MATRIX_LOG("[matrix_qr_decomposition] Entering function");

    if (!A || A->rows < A->cols) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: Matrix must have more rows than columns.");
        return false;
    }

    size_t m = A->rows;
    size_t n = A->cols;

    *Q = matrix_create(m, n);
    *R = matrix_create(n, n);
    if (!*Q || !*R) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: Memory allocation failed for Q and R.");
        return false;
    }

    // Log memory allocation for temporary vectors
    MATRIX_LOG("[matrix_qr_decomposition] Allocating memory for temporary vectors.");
    double* a_col = (double*)malloc(sizeof(double) * m);
    double* q_col = (double*)malloc(sizeof(double) * m);
    if (!a_col || !q_col) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: Memory allocation failed for column vectors.");
        return false;
    }

    for (size_t i = 0; i < n; ++i) {
        // Copy the ith column of A to a_col
        for (size_t j = 0; j < m; ++j) {
            a_col[j] = matrix_get(A, j, i);
        }

        for (size_t k = 0; k < i; ++k) {
            for (size_t j = 0; j < m; ++j) {
                q_col[j] = matrix_get(*Q, j, k);
            }
            subtract_projection(a_col, q_col, m);  // a_col -= projection of a_col onto q_col
        }

        normalize_vector(a_col, m);  

        for (size_t j = 0; j < m; ++j) {
            matrix_set(*Q, j, i, a_col[j]);
        }

        MATRIX_LOG("[matrix_qr_decomposition] Set column %zu of Q", i);
    }

    for (size_t j = 0; j < n; ++j) {
        for (size_t i = 0; i <= j; ++i) {
            double r_ij = 0.0;
            for (size_t k = 0; k < m; ++k) {
                r_ij += matrix_get(*Q, k, i) * matrix_get(A, k, j);
            }
            matrix_set(*R, i, j, r_ij);
            MATRIX_LOG("[matrix_qr_decomposition] Set R(%zu, %zu) = %lf", i, j, r_ij);
        }
    }

    free(a_col);
    free(q_col);

    MATRIX_LOG("[matrix_qr_decomposition] Exiting function.");
    return true;
}

/**
 * @brief Creates a Pascal matrix of size n x n.
 *
 * This function generates a Pascal matrix, which is a symmetric matrix where each element is a binomial coefficient.
 * The matrix is filled in both the upper and lower triangular parts using the binomial coefficients.
 *
 * @param n The size of the Pascal matrix (number of rows and columns).
 * @return A pointer to the generated Pascal matrix. Returns `NULL` if memory allocation fails.
 */
Matrix* matrix_pascal(size_t n) {
    MATRIX_LOG("[matrix_pascal] Entering function with size %zu", n);

    Matrix* pascalMatrix = matrix_create(n, n);
    if (!pascalMatrix) {
        MATRIX_LOG("[matrix_pascal] Error: Memory allocation failed for Pascal matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j <= i; j++) {
            double value = binomial_coefficient(i + j, i);
            matrix_set(pascalMatrix, i, j, value);
            matrix_set(pascalMatrix, j, i, value);

            MATRIX_LOG("[matrix_pascal] Set Pascal matrix element (%zu, %zu) = %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_pascal] Successfully created Pascal matrix.");
    return pascalMatrix;
}

/**
 * @brief Computes the Frobenius norm of a matrix.
 *
 * The Frobenius norm is calculated as the square root of the sum of the absolute squares of all elements in the matrix.
 * It provides a measure of the magnitude of the matrix.
 *
 * @param matrix The input matrix for which the Frobenius norm is to be computed.
 * @return The Frobenius norm of the matrix.
 */
double matrix_frobenius_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_frobenius_norm] Entering function");

    double sum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            double value = matrix_get(matrix, i, j);
            sum += value * value;

            MATRIX_LOG("[matrix_frobenius_norm] Adding value^2 for element (%zu, %zu): %lf", i, j, value * value);
        }
    }

    double frobeniusNorm = sqrt(sum);

    MATRIX_LOG("[matrix_frobenius_norm] Frobenius norm = %lf", frobeniusNorm);
    return frobeniusNorm;
}

/**
 * @brief Computes the L1 norm of a matrix.
 *
 * The L1 norm, also known as the maximum column sum norm, is calculated as the maximum of the sums of absolute values of each column.
 *
 * @param matrix The input matrix for which the L1 norm is to be computed.
 * @return The L1 norm of the matrix.
 */
double matrix_l1_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_l1_norm] Entering function");

    double maxSum = 0.0;
    for (size_t j = 0; j < matrix->cols; j++) {
        double columnSum = 0.0;
        for (size_t i = 0; i < matrix->rows; i++) {
            columnSum += fabs(matrix_get(matrix, i, j));
        }
        MATRIX_LOG("[matrix_l1_norm] Column %zu sum = %f", j, columnSum);

        if (columnSum > maxSum) {
            maxSum = columnSum;
            MATRIX_LOG("[matrix_l1_norm] New max column sum = %f", maxSum);
        }
    }

    MATRIX_LOG("[matrix_l1_norm] Exiting function, L1 norm = %f", maxSum);
    return maxSum;
}

/**
 * @brief Computes the infinity norm of a matrix.
 *
 * The infinity norm, also known as the maximum row sum norm, is calculated as the maximum of the sums of absolute values of each row.
 *
 * @param matrix The input matrix for which the infinity norm is to be computed.
 * @return The infinity norm of the matrix.
 */
double matrix_infinity_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_infinity_norm] Entering function");

    double maxSum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        double rowSum = 0.0;
        for (size_t j = 0; j < matrix->cols; j++) {
            rowSum += fabs(matrix_get(matrix, i, j));
        }
        MATRIX_LOG("[matrix_infinity_norm] Row %zu sum = %f", i, rowSum);

        if (rowSum > maxSum) {
            maxSum = rowSum;
            MATRIX_LOG("[matrix_infinity_norm] New max row sum = %f", maxSum);
        }
    }

    MATRIX_LOG("[matrix_infinity_norm] Exiting function, infinity norm = %f", maxSum);
    return maxSum;
}

/**
 * @brief Computes the inverse of a square matrix using the Gauss-Jordan elimination method.
 *
 * This function takes a square matrix and computes its inverse by performing Gauss-Jordan elimination. 
 * If the matrix is not square or is singular (non-invertible), the function returns `NULL`.
 *
 * @param matrix The input square matrix to be inverted.
 * @return A pointer to the inverse matrix. If the matrix is not invertible or an error occurs, `NULL` is returned.
 */
Matrix* matrix_inverse_gauss_jordan(const Matrix* matrix) {
    MATRIX_LOG("[matrix_inverse_gauss_jordan] Entering function");

    if (matrix->rows != matrix->cols) {
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Matrix must be square for inversion.");
        return NULL;
    }

    size_t n = matrix->rows;
    MATRIX_LOG("[matrix_inverse_gauss_jordan] Creating augmented matrix of size %zux%zu", n, 2 * n);

    Matrix* augmented = matrix_create(n, 2 * n);
    if (!augmented) {
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Memory allocation failed for augmented matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            augmented->data[i * 2 * n + j] = matrix->data[i * n + j]; 
            augmented->data[i * 2 * n + j + n] = (i == j) ? 1.0 : 0.0; 
        }
    }

    // Perform Gauss-Jordan elimination
    for (size_t col = 0; col < n; col++) {
        if (augmented->data[col * 2 * n + col] == 0) {
            MATRIX_LOG("[matrix_inverse_gauss_jordan] Pivot element is zero, searching for non-zero element to swap.");
            size_t swapRow = col + 1;
            while (swapRow < n && augmented->data[swapRow * 2 * n + col] == 0) {
                swapRow++;
            }
            if (swapRow == n) {
                MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Matrix is singular, cannot invert.");
                matrix_deallocate(augmented);
                return NULL;
            }
            MATRIX_LOG("[matrix_inverse_gauss_jordan] Swapping row %zu with row %zu", col, swapRow);
            matrix_swap_rows(augmented, col, swapRow);
        }

        matrix_row_divide(augmented, col, augmented->data[col * 2 * n + col]);
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Divided row %zu by pivot element", col);

        for (size_t row = 0; row < n; row++) {
            if (row != col) {
                matrix_row_subtract(augmented, row, col, augmented->data[row * 2 * n + col]);
                MATRIX_LOG("[matrix_inverse_gauss_jordan] Eliminated column %zu in row %zu", col, row);
            }
        }
    }

    Matrix* inverse = matrix_create(n, n);
    if (!inverse) {
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Memory allocation failed for inverse matrix.");
        matrix_deallocate(augmented);
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            inverse->data[i * n + j] = augmented->data[i * 2 * n + j + n];
        }
    }

    MATRIX_LOG("[matrix_inverse_gauss_jordan] Successfully computed inverse matrix.");
    matrix_deallocate(augmented);
    return inverse;
}

/**
 * @brief Checks if a matrix is positive definite.
 *
 * This function verifies if a given matrix is positive definite. For a matrix to be positive definite, 
 * it must be symmetric and its Cholesky decomposition must exist. If the matrix is not square or not symmetric,
 * the function returns `false`.
 *
 * @param matrix The input matrix to check for positive definiteness.
 * 
 * @return `true` if the matrix is positive definite, otherwise `false`.
 */
bool matrix_is_positive_definite(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_positive_definite] Entering function");

    if (!matrix || matrix->rows != matrix->cols) {
        MATRIX_LOG("[matrix_is_positive_definite] Error: Matrix must be square to check if it's positive definite.");
        return false;
    }
    if (!matrix_is_symmetric(matrix)) {
        MATRIX_LOG("[matrix_is_positive_definite] Error: Matrix is not symmetric.");
        return false;
    }

    MATRIX_LOG("[matrix_is_positive_definite] Attempting Cholesky decomposition.");
    Matrix* chol = matrix_cholesky_decomposition(matrix);
    bool isPositiveDefinite = chol != NULL;

    if (isPositiveDefinite) {
        MATRIX_LOG("[matrix_is_positive_definite] Matrix is positive definite.");
        matrix_deallocate(chol); 
    }
    else {
        MATRIX_LOG("[matrix_is_positive_definite] Error: Cholesky decomposition failed.");
        return false;
    }

    MATRIX_LOG("[matrix_is_positive_definite] Exiting function.");
    return isPositiveDefinite;
}

/**
 * @brief Calculates the projection matrix onto the column space of a given matrix.
 *
 * This function computes the projection matrix P that projects any vector onto the column space of the input matrix A.
 *
 * @param matrix The input matrix A.
 * 
 * @return A pointer to the projection matrix. If any step in the calculation fails (memory allocation, matrix inversion),
 * `NULL` is returned.
 */
Matrix* matrix_projection(const Matrix* matrix) {
    MATRIX_LOG("[matrix_projection] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_projection] Error: Input matrix is null.");
        return NULL;
    }

    MATRIX_LOG("[matrix_projection] Calculating transpose.");
    Matrix* matrixTranspose = matrix_transpose(matrix);
    if (!matrixTranspose) {
        MATRIX_LOG("[matrix_projection] Error: Transpose calculation failed.");
        return NULL;
    }

    MATRIX_LOG("[matrix_projection] Calculating matrix^T * matrix.");
    Matrix* mta = matrix_multiply(matrixTranspose, matrix);
    if (!mta) {
        MATRIX_LOG("[matrix_projection] Error: Multiplication matrix^T * matrix failed.");
        matrix_deallocate(matrixTranspose);
        return NULL;
    }

    // Calculate inverse of m^T * m
    MATRIX_LOG("[matrix_projection] Calculating inverse of matrix^T * matrix.");
    Matrix* mtaInv = matrix_inverse(mta);
    if (!mtaInv) {
        MATRIX_LOG("[matrix_projection] Error: Inverse calculation failed.");
        matrix_deallocate(matrixTranspose); 
        matrix_deallocate(mta); 
        return NULL;
    }

    MATRIX_LOG("[matrix_projection] Calculating A * (A^T * A)^-1.");
    Matrix* m_mta_inv = matrix_multiply(matrix, mtaInv);

    if (!m_mta_inv) {
        MATRIX_LOG("[matrix_projection] Error: Multiplication A * (A^T * A)^-1 failed.");
        matrix_deallocate(matrixTranspose);
        matrix_deallocate(mta);
        matrix_deallocate(mtaInv);
        return NULL;
    }

    // Calculate final projection matrix: A * (A^T * A)^-1 * A^T
    MATRIX_LOG("[matrix_projection] Calculating final projection matrix.");
    Matrix* projection = matrix_multiply(m_mta_inv, matrixTranspose);

    if (!projection) {
        MATRIX_LOG("[matrix_projection] Error: Final projection matrix calculation failed.");
    } 
    else {
        MATRIX_LOG("[matrix_projection] Successfully calculated projection matrix.");
    }

    matrix_deallocate(matrixTranspose);
    matrix_deallocate(mta);
    matrix_deallocate(mtaInv);
    matrix_deallocate(m_mta_inv);

    return projection;
}

/**
 * @brief Generates a Vandermonde matrix from the given input matrix.
 *
 * A Vandermonde matrix is a matrix with terms of a geometric progression in each row. 
 * 
 * @param matrix The input matrix containing the initial values for the Vandermonde matrix.
 * @param n The size (rows and columns) of the resulting Vandermonde matrix.
 * 
 * @return A pointer to the generated Vandermonde matrix. If the input matrix is `NULL` or 
 * memory allocation fails, `NULL` is returned.
 */
Matrix* matrix_vandermonde(const Matrix* matrix, size_t n) {
    MATRIX_LOG("[matrix_vandermonde] Entering function with n = %zu", n);

    if (!matrix) {
        MATRIX_LOG("[matrix_vandermonde] Error: Matrix object is null");
        return NULL;
    }
    
    Matrix* vandermonde = matrix_create(n, n);
    if (!vandermonde) {
        MATRIX_LOG("[matrix_vandermonde] Error: Memory allocation failed for Vandermonde matrix");
        return NULL; 
    }

    MATRIX_LOG("[matrix_vandermonde] Successfully created Vandermonde matrix of size %zux%zu", n, n);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            vandermonde->data[i * n + j] = pow(matrix->data[i], j);
            MATRIX_LOG("[matrix_vandermonde] Setting element at (row, col) = (%zu, %zu) to %lf", i, j, vandermonde->data[i * n + j]);
        }
    }

    MATRIX_LOG("[matrix_vandermonde] Successfully created Vandermonde matrix");
    return vandermonde;
}

/**
 * @brief Generates a companion matrix from the given polynomial coefficients.
 *
 * @param coefficients A matrix representing the polynomial coefficients, where the highest degree coefficient comes last.
 * @param degree The degree of the polynomial.
 * 
 * @return A pointer to the generated companion matrix. If the input coefficients are `NULL` or memory allocation fails,
 * `NULL` is returned.
 */
Matrix* matrix_companion(const Matrix* coefficients, size_t degree) {
    MATRIX_LOG("[matrix_companion] Entering function with degree = %zu", degree);

    if (!coefficients) {
        MATRIX_LOG("[matrix_companion] Error: Coefficients matrix is null");
        return NULL;
    }
    
    size_t n = degree - 1; 
    Matrix* companion = matrix_create(n, n);
    if (!companion) {
        MATRIX_LOG("[matrix_companion] Error: Memory allocation failed for companion matrix");
        return NULL;
    }

    MATRIX_LOG("[matrix_companion] Successfully created companion matrix of size %zux%zu", n, n);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (j == n - 1) { 
                companion->data[i * n + j] = -coefficients->data[n - 1 - i] / coefficients->data[degree - 1];
                MATRIX_LOG("[matrix_companion] Setting element at (row, col) = (%zu, %zu) to %lf (last column)", i, j, companion->data[i * n + j]);
            } 
            else if (i == j + 1) { 
                companion->data[i * n + j] = 1;
                MATRIX_LOG("[matrix_companion] Setting element at (row, col) = (%zu, %zu) to 1 (sub-diagonal)", i, j);
            } 
            else { 
                companion->data[i * n + j] = 0;
                MATRIX_LOG("[matrix_companion] Setting element at (row, col) = (%zu, %zu) to 0", i, j);
            }
        }
    }

    MATRIX_LOG("[matrix_companion] Successfully created companion matrix");
    return companion;
}

/**
 * @brief Fills a matrix with a specified value.
 *
 * This function sets every element of the matrix to the provided value.
 *
 * @param matrix The matrix to be filled.
 * @param value The value to fill the matrix with.
 * 
 * @return `true` if the matrix was successfully filled, `false` if the matrix or its data pointer is `NULL`.
 */
bool matrix_fill(Matrix* matrix, double value) {
    MATRIX_LOG("[matrix_fill] Entering function with value = %lf", value);

    if (!matrix) {
        MATRIX_LOG("[matrix_fill] Error: Matrix object is null");
        return false; 
    }
    if (!matrix->data) {
        MATRIX_LOG("[matrix_fill] Error: Matrix data is null");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            matrix->data[i * matrix->cols + j] = value;
            MATRIX_LOG("[matrix_fill] Setting element at (row, col) = (%zu, %zu) to %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_fill] Successfully filled matrix with value %lf", value);
    return true;
}

/**
 * @brief Applies a specified function to each element of a matrix.
 *
 * This function creates a new matrix where each element is the result of applying the provided function `func`
 * to the corresponding element of the input matrix.
 *
 * @param matrix The input matrix to which the function will be applied.
 * @param func The function to apply to each element of the matrix.
 * 
 * @return A new matrix where each element is the result of applying `func` to the input matrix's elements.
 * If the input matrix or function is `NULL`, or if memory allocation fails, the function returns `NULL`.
 */
Matrix* matrix_map(const Matrix* matrix, MatrixFunc func) {
    MATRIX_LOG("[matrix_map] Entering function");

    if (!matrix || !func) {
        MATRIX_LOG("[matrix_map] Error: NULL argument provided");
        return NULL;
    }

    Matrix* result = (Matrix*)malloc(sizeof(Matrix));
    if (!result) {
        MATRIX_LOG("[matrix_map] Error: Memory allocation failed for matrix structure");
        return NULL;
    }

    result->data = (double*)malloc(matrix->rows * matrix->cols * sizeof(double));
    if (!result->data) {
        MATRIX_LOG("[matrix_map] Error: Memory allocation for matrix data failed");
        free(result); 
        return NULL;
    }

    result->rows = matrix->rows;
    result->cols = matrix->cols;

    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            MATRIX_LOG("[matrix_map] Applying function to element at (row, col) = (%zu, %zu)", i, j);
            result->data[i * matrix->cols + j] = func(matrix->data[i * matrix->cols + j]);
        }
    }

    MATRIX_LOG("[matrix_map] Successfully applied function to matrix");
    return result;
}

/**
 * @brief Finds the minimum element in a matrix.
 *
 * This function iterates through all the elements of the matrix to find and return the minimum value.
 *
 * @param matrix The matrix in which to find the minimum element.
 * 
 * @return The minimum element in the matrix. If the matrix is `NULL`, empty, or invalid, 
 * it returns `DBL_MAX` as an error indicator.
 */
double matrix_min_element(const Matrix* matrix) {
    MATRIX_LOG("[matrix_min_element] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_min_element] Error: Invalid matrix provided");
        return DBL_MAX; 
    }

    double min = DBL_MAX;
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
            if (value < min) {
                MATRIX_LOG("[matrix_min_element] New minimum found: %f at (row, col) = (%zu, %zu)", value, i, j);
                min = value;
            }
        }
    }

    MATRIX_LOG("[matrix_min_element] Minimum element found: %f", min);
    return min;
}


/**
 * @brief Finds the maximum element in a matrix.
 *
 * This function iterates through all the elements of the matrix to find and return the maximum value.
 *
 * @param matrix The matrix in which to find the maximum element.
 * 
 * @return The maximum element in the matrix. If the matrix is `NULL`, empty, or invalid, it returns `-DBL_MAX` as an error indicator.
 */
double matrix_max_element(const Matrix* matrix) {
    MATRIX_LOG("[matrix_max_element] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_max_element] Error: Invalid matrix provided");
        return -DBL_MAX; 
    }

    double max = -DBL_MAX;
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
            if (value > max) {
                MATRIX_LOG("[matrix_max_element] New maximum found: %f at (row, col) = (%zu, %zu)", value, i, j);
                max = value;
            }
        }
    }

    MATRIX_LOG("[matrix_max_element] Maximum element found: %f", max);
    return max;
}

/**
 * @brief Applies a function to each element of a specified row in a matrix.
 *
 * This function applies the provided function `func` to each element of the specified row in the matrix, 
 * modifying the row in place.
 *
 * @param matrix The matrix whose row will be modified.
 * @param row The index of the row to which the function will be applied.
 * @param func The function to apply to each element of the specified row.
 * 
 * @return `true` if the operation is successful, `false` if the matrix or function is `NULL`, 
 * or the row index is out of bounds.
 */
bool matrix_apply_to_row(Matrix* matrix, size_t row, MatrixFunc func) {
    MATRIX_LOG("[matrix_apply_to_row] Entering function with row = %zu", row);

    if (!matrix || !func || row >= matrix->rows) {
        MATRIX_LOG("[matrix_apply_to_row] Error: Invalid arguments. Matrix is NULL or row index out of bounds.");
        return false;
    }
    for (size_t j = 0; j < matrix->cols; ++j) {
        MATRIX_LOG("[matrix_apply_to_row] Applying function to element at (row, col) = (%zu, %zu)", row, j);
        matrix->data[row * matrix->cols + j] = func(matrix->data[row * matrix->cols + j]);
    }

    MATRIX_LOG("[matrix_apply_to_row] Successfully applied function to row %zu", row);
    return true;
}

/**
 * @brief Applies a function to each element of a specified column in a matrix.
 *
 * This function applies the provided function `func` to each element of the specified column in the matrix, 
 * modifying the column in place.
 *
 * @param matrix The matrix whose column will be modified.
 * @param col The index of the column to which the function will be applied.
 * @param func The function to apply to each element of the specified column.
 * 
 * @return `true` if the operation is successful, `false` if the matrix or function is `NULL`, 
 * or the column index is out of bounds.
 */
bool matrix_apply_to_col(Matrix* matrix, size_t col, MatrixFunc func) {
    MATRIX_LOG("[matrix_apply_to_col] Entering function with col = %zu", col);

    if (!matrix || !func || col >= matrix->cols) {
        MATRIX_LOG("[matrix_apply_to_col] Error: Invalid arguments. Matrix is NULL or column index out of bounds.");
        return false;
    }
    for (size_t i = 0; i < matrix->rows; ++i) {
        MATRIX_LOG("[matrix_apply_to_col] Applying function to element at (row, col) = (%zu, %zu)", i, col);
        matrix->data[i * matrix->cols + col] = func(matrix->data[i * matrix->cols + col]);
    }

    MATRIX_LOG("[matrix_apply_to_col] Successfully applied function to column %zu", col);
    return true;
}

/**
 * @brief Adds one row of a matrix to another, optionally scaling the source row before adding.
 *
 * This function adds the elements of the `sourceRow` to the corresponding elements of the `targetRow`
 * in the matrix, optionally scaling the source row by a specified factor before the addition.
 *
 * @param matrix The matrix in which the row addition will take place.
 * @param targetRow The index of the row that will be updated (the row to which the source row is added).
 * @param sourceRow The index of the row that will be added to the target row.
 * @param scale The scaling factor by which to multiply the source row before adding. Use 1.0 for no scaling.
 * 
 * @return `true` if the operation is successful, `false` if the matrix is `NULL` or the row indices are invalid.
 */
bool matrix_row_addition(Matrix* matrix, size_t targetRow, size_t sourceRow, double scale) {
    MATRIX_LOG("[matrix_row_addition] Entering function with targetRow = %zu, sourceRow = %zu, scale = %f", targetRow, sourceRow, scale);

    if (!matrix || targetRow >= matrix->rows || sourceRow >= matrix->rows) {
        MATRIX_LOG("[matrix_row_addition] Error: Invalid arguments. Matrix is NULL or row indices out of bounds.");
        return false;
    }
    for (size_t j = 0; j < matrix->cols; ++j) {
        MATRIX_LOG("[matrix_row_addition] Adding scaled sourceRow (%zu) element at (row, col) = (%zu, %zu) to targetRow (%zu)", sourceRow, sourceRow, j, targetRow);
        matrix->data[targetRow * matrix->cols + j] += scale * matrix->data[sourceRow * matrix->cols + j];
    }

    MATRIX_LOG("[matrix_row_addition] Successfully added row %zu to row %zu", sourceRow, targetRow);
    return true;
}

/**
 * @brief Adds one column of a matrix to another, optionally scaling the source column before adding.
 *
 * This function adds the elements of the `sourceCol` to the corresponding elements of the `targetCol`
 * in the matrix, optionally scaling the source column by a specified factor before the addition.
 *
 * @param matrix The matrix in which the column addition will take place.
 * @param targetCol The index of the column that will be updated (the column to which the source column is added).
 * @param sourceCol The index of the column that will be added to the target column.
 * @param scale The scaling factor by which to multiply the source column before adding. Use 1.0 for no scaling.
 * 
 * @return `true` if the operation is successful, `false` if the matrix is `NULL` or the column indices are invalid.
 */
bool matrix_col_addition(Matrix* matrix, size_t targetCol, size_t sourceCol, double scale) {
    MATRIX_LOG("[matrix_col_addition] Entering function with targetCol = %zu, sourceCol = %zu, scale = %f", targetCol, sourceCol, scale);

    if (!matrix || targetCol >= matrix->cols || sourceCol >= matrix->cols) {
        MATRIX_LOG("[matrix_col_addition] Error: Invalid arguments. Matrix is NULL or column indices out of bounds.");
        return false;
    }
    for (size_t i = 0; i < matrix->rows; ++i) {
        MATRIX_LOG("[matrix_col_addition] Adding scaled sourceCol (%zu) element at (row, col) = (%zu, %zu) to targetCol (%zu)", sourceCol, i, sourceCol, targetCol);
        matrix->data[i * matrix->cols + targetCol] += scale * matrix->data[i * matrix->cols + sourceCol];
    }

    MATRIX_LOG("[matrix_col_addition] Successfully added column %zu to column %zu", sourceCol, targetCol);
    return true;
}

/**
 * @brief Creates a Leslie matrix using fecundity and survival coefficients.
 *
 * A Leslie matrix is a type of matrix used in population ecology to model the dynamics of a population 
 * with different age classes. The first row contains fecundity coefficients, and the sub-diagonal contains 
 * survival coefficients.
 *
 * @param f A matrix representing the fecundity coefficients.
 * @param f_size The number of fecundity coefficients (the number of columns in the Leslie matrix).
 * @param s A matrix representing the survival coefficients.
 * @param s_size The number of survival coefficients (one less than the number of rows in the Leslie matrix).
 * 
 * @return A pointer to the newly created Leslie matrix, or `NULL` if the input parameters are invalid 
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_leslie(Matrix* f, size_t f_size, Matrix* s, size_t s_size) {
    MATRIX_LOG("[matrix_leslie] Entering function with f_size = %zu, s_size = %zu", f_size, s_size);

    if (!f) {
        MATRIX_LOG("[matrix_leslie] Error: Matrix f is null");
        return NULL;
    }
    if (f_size != s_size + 1) {
        MATRIX_LOG("[matrix_leslie] Error: f_size (%zu) must be one more than s_size (%zu)", f_size, s_size);
        return NULL;
    }

    // Create an N x N matrix, where N is the size of the fecundity array f
    MATRIX_LOG("[matrix_leslie] Creating Leslie matrix of size %zux%zu", f_size, f_size);
    Matrix* leslie = matrix_create(f_size, f_size);

    if (!leslie) {
        MATRIX_LOG("[matrix_leslie] Error: Memory allocation failed for Leslie matrix");
        return NULL;
    }

    // Set the first row with fecundity coefficients
    for (size_t i = 0; i < f_size; ++i) {
        MATRIX_LOG("[matrix_leslie] Setting fecundity coefficient at position (0, %zu)", i);
        matrix_set(leslie, 0, i, f->data[i]);
    }
    for (size_t i = 1; i < f_size; ++i) {
        MATRIX_LOG("[matrix_leslie] Setting survival coefficient at position (%zu, %zu)", i, i - 1);
        matrix_set(leslie, i, i - 1, s->data[i - 1]);
    }

    MATRIX_LOG("[matrix_leslie] Successfully created Leslie matrix");

    return leslie;
}

/**
 * @brief Creates a Fiedler matrix from a given matrix.
 *
 * This function generates a Fiedler matrix, which is a symmetric matrix where each element 
 * at position (i, j) is the absolute difference between the elements at positions i and j of 
 * the input matrix's data array.
 *
 * @param matrix The input matrix whose elements will be used to generate the Fiedler matrix.
 * @return A pointer to the newly created Fiedler matrix, or `NULL` if the input matrix is `NULL`
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_fiedler(const Matrix* matrix) {
    MATRIX_LOG("[matrix_fiedler] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_fiedler] Error: Matrix object is null");
        return NULL;
    }

    size_t n = matrix->cols >= matrix->rows ? matrix->cols : matrix->rows;
    MATRIX_LOG("[matrix_fiedler] Creating Fiedler matrix of size %zux%zu", n, n);

    Matrix* fiedler = matrix_create(n, n);
    if (!fiedler) {
        MATRIX_LOG("[matrix_fiedler] Error: Memory allocation failed for Fiedler matrix");
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double value = fabs(matrix->data[i] - matrix->data[j]);
            MATRIX_LOG("[matrix_fiedler] Setting element at (%zu, %zu) to %f", i, j, value);
            matrix_set(fiedler, i, j, value);
        }
    }

    MATRIX_LOG("[matrix_fiedler] Successfully created Fiedler matrix");
    return fiedler;
}


/**
 * @brief Creates the inverse of a Hilbert matrix of size `n`.
 *
 * This function generates the inverse of a Hilbert matrix, which is a square matrix with 
 * elements defined by the formula for the inverse Hilbert matrix. The function uses binomial 
 * coefficients and factorials to calculate the values.
 *
 * @param n The size of the Hilbert matrix to be inverted.
 * 
 * @return A pointer to the newly created inverse Hilbert matrix, or `NULL` if memory allocation fails. 
 * The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_inverse_hilbert(size_t n) {
    MATRIX_LOG("[matrix_inverse_hilbert] Entering function with n = %zu", n);
    Matrix* invH = matrix_create(n, n);

    if (!invH) {
        MATRIX_LOG("[matrix_inverse_hilbert] Error: Memory allocation failed for inverse Hilbert matrix");
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            int s = i + j;
            int64_t sign = (s % 2 == 0) ? 1 : -1;
            int64_t numerator = sign * (i + j + 1) * binomial_factorial(n + i, n - j - 1) * binomial_factorial(n + j, n - i - 1) * binomial_factorial(s, i) * binomial_factorial(s, j);
            int64_t denominator = 1; // The denominator for Hilbert matrix inverse entries when n <= 14 are effectively 1
            double value = (double)numerator / denominator;

            MATRIX_LOG("[matrix_inverse_hilbert] Setting element at (%zu, %zu) to %f", i, j, value);
            matrix_set(invH, i, j, value);
        }
    }

    MATRIX_LOG("[matrix_inverse_hilbert] Successfully created inverse Hilbert matrix");
    return invH;
}


/**
 * @brief Extracts a specific row from a matrix and returns it as a new matrix.
 *
 * This function creates a new matrix containing the elements of the specified row from the input matrix.
 *
 * @param matrix The input matrix from which the row is to be extracted.
 * @param row The index of the row to extract (0-based).
 * 
 * @return A pointer to the newly created matrix containing the specified row, or `NULL` if the input matrix is `NULL`, the row index is out of bounds,
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_get_row(const Matrix* matrix, size_t row) {
    MATRIX_LOG("[matrix_get_row] Entering function with row = %zu", row);

    if (!matrix) {
        MATRIX_LOG("[matrix_get_row] Error: Matrix object is null or invalid");
        return NULL;
    }
    else if (row >= matrix->rows) {
        MATRIX_LOG("[matrix_get_row] Error: Row index (%zu) out of bounds, matrix has %zu rows", row, matrix->rows);
        return NULL;
    }

    MATRIX_LOG("[matrix_get_row] Creating row matrix with 1 row and %zu columns", matrix->cols);

    Matrix* r = matrix_create(1, matrix->cols);
    if (!r) {
        MATRIX_LOG("[matrix_get_row] Error: Memory allocation failed for row");
        return NULL;
    }
    
    for (size_t j = 0; j < matrix->cols; j++) {
        MATRIX_LOG("[matrix_get_row] Extracting element at (row, col) = (%zu, %zu)", row, j);
        matrix_set(r, 0, j, matrix_get(matrix, row, j));
    }
    MATRIX_LOG("[matrix_get_row] Successfully created row matrix");

    return r;
}

/**
 * @brief Extracts a specific column from a matrix and returns it as a new matrix.
 *
 * This function creates a new matrix containing the elements of the specified column from the input matrix.
 *
 * @param matrix The input matrix from which the column is to be extracted.
 * @param col The index of the column to extract (0-based).
 * 
 * @return A pointer to the newly created matrix containing the specified column, or `NULL` if the input matrix is `NULL`, the column index is out of bounds,
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_get_col(const Matrix* matrix, size_t col) {
    MATRIX_LOG("[matrix_get_col] Entering function with col = %zu", col);

    if (!matrix) {
        MATRIX_LOG("[matrix_get_col] Error: Matrix object is null or invalid");
        return NULL;
    }
    else if (col >= matrix->cols) {
        MATRIX_LOG("[matrix_get_col] Error: Column index (%zu) out of bounds, matrix has %zu columns", col, matrix->cols);
        return NULL;
    }
    MATRIX_LOG("[matrix_get_col] Creating column matrix with %zu rows and 1 column", matrix->rows);

    Matrix* c = matrix_create(matrix->rows, 1);
    if (!c) {
        MATRIX_LOG("[matrix_get_col] Error: Memory allocation failed for column");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        MATRIX_LOG("[matrix_get_col] Extracting element at (row, col) = (%zu, %zu)", i, col);
        matrix_set(c, i, 0, matrix_get(matrix, i, col));
    }
    MATRIX_LOG("[matrix_get_col] Successfully created column matrix");

    return c;
}

/**
 * @brief Converts a matrix to a 1D array of doubles.
 *
 * This function copies the elements of the matrix into a newly allocated 1D array.
 * The array is a linear representation of the matrix data in row-major order.
 *
 * @param matrix The matrix to be converted to an array.
 * 
 * @return A pointer to the newly allocated array containing the matrix elements, or `NULL` if the matrix is `NULL` or memory allocation fails. 
 * The caller is responsible for freeing the allocated memory.
 */
double* matrix_to_array(const Matrix* matrix) {
    MATRIX_LOG("[matrix_to_array] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_to_array] Error: Matrix object is null or invalid");
        return NULL;
    }

    size_t size = matrix->rows * matrix->cols * sizeof(double);
    MATRIX_LOG("[matrix_to_array] Allocating array of size %zu bytes", size);

    double* data = (double*) malloc(size);
    if (!data) {
        MATRIX_LOG("[matrix_to_array] Error: Memory allocation failed");
        return NULL;
    }

    MATRIX_LOG("[matrix_to_array] Copying matrix data to array");
    memcpy(data, matrix->data, size);
    MATRIX_LOG("[matrix_to_array] Successfully converted matrix to array");

    return data;
}

/**
 * @brief Creates a block diagonal matrix from a variable number of matrices.
 *
 * This function constructs a block diagonal matrix by placing each provided matrix
 * along the diagonal of a larger matrix, leaving the off-diagonal blocks as zero matrices.
 *
 * @param count The number of matrices to include in the block diagonal matrix.
 * @param ... A variable number of `Matrix*` pointers, each representing a block to be placed on the diagonal.
 * 
 * @return A pointer to the newly created block diagonal matrix, or `NULL` if memory allocation fails.
 * The caller is responsible for freeing the allocated matrix using `matrix_deallocate`.
 */
Matrix* matrix_block_diag(size_t count, ...) {
    MATRIX_LOG("[matrix_block_diag] Entering function with count = %zu", count);

    va_list args;
    size_t totalRows = 0, totalCols = 0;

    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        Matrix* mat = va_arg(args, Matrix*);
        totalRows += mat->rows;
        totalCols += mat->cols;
        
        MATRIX_LOG("[matrix_block_diag] Matrix %zu: %zux%zu", i + 1, mat->rows, mat->cols);
    }
    va_end(args);
    MATRIX_LOG("[matrix_block_diag] Total matrix size: %zux%zu", totalRows, totalCols);

    Matrix* result = matrix_create(totalRows, totalCols);
    if (!result) {
        MATRIX_LOG("[matrix_block_diag] Error: Memory allocation failed for block diagonal matrix");
        return NULL;
    }

    size_t currentRow = 0, currentCol = 0;
    va_start(args, count);

    for (size_t i = 0; i < count; ++i) {
        Matrix* mat = va_arg(args, Matrix*);
        for (size_t r = 0; r < mat->rows; ++r) {
            // Log data copying process for each matrix block
            MATRIX_LOG("[matrix_block_diag] Copying data for matrix %zu into block diagonal matrix at position (%zu, %zu)", i + 1, currentRow, currentCol);
            memcpy(result->data + (currentRow + r) * totalCols + currentCol, mat->data + r * mat->cols, mat->cols * sizeof(double));
        }
        currentRow += mat->rows;
        currentCol += mat->cols;
    }

    va_end(args);
    MATRIX_LOG("[matrix_block_diag] Successfully created block diagonal matrix");

    return result;
}

/**
 * @brief Determines if a matrix is sparse.
 *
 * A matrix is considered sparse if a significant portion of its elements are zero.
 * This function calculates the percentage of non-zero elements and considers 
 * the matrix sparse if less than 30% of the elements are non-zero.
 *
 * @param matrix The matrix to check for sparsity.
 * @return `true` if the matrix is sparse, otherwise `false`.
 */
bool matrix_is_sparse(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_sparse] Entering function");
    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_is_sparse] Error: Matrix object is null or invalid");
        return false;
    }

    size_t totalElements = matrix->rows * matrix->cols;
    size_t nonZeroCount = 0;

    MATRIX_LOG("[matrix_is_sparse] Matrix size: %zux%zu (%zu total elements)", matrix->rows, matrix->cols, totalElements);
    for (size_t i = 0; i < totalElements; ++i) {
        if (matrix->data[i] != 0) {
            ++nonZeroCount;
        }
    }
    MATRIX_LOG("[matrix_is_sparse] Non-zero elements count: %zu", nonZeroCount);

    double nonZeroPercentage = (double)nonZeroCount / (double)totalElements;
    MATRIX_LOG("[matrix_is_sparse] Non-zero percentage: %.2f%%", nonZeroPercentage * 100);

    bool isSparse = nonZeroPercentage < 0.3;
    MATRIX_LOG("[matrix_is_sparse] Matrix is %s", isSparse ? "sparse" : "not sparse");

    return isSparse;
}

/**
 * @brief Returns the total number of elements in a matrix.
 *
 * This function calculates the size of the matrix, defined as the product 
 * of its rows and columns.
 *
 * @param matrix The matrix whose size is to be determined.
 * @return The total number of elements in the matrix, or 0 if the matrix is NULL.
 */
size_t matrix_size(const Matrix *matrix) {
    MATRIX_LOG("[matrix_size] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_size] Error: Matrix object is null or invalid");
        return 0;
    }

    size_t size = matrix->rows * matrix->cols;
    MATRIX_LOG("[matrix_size] Matrix size: %zu", size);

    return size;
}

/**
 * @brief Creates a matrix with random integer values.
 *
 * This function generates a matrix of the specified size (row x col) and fills it 
 * with random integers within the specified range [start, end). The random values 
 * are generated using the standard C library's `rand()` function.
 *
 * @param row The number of rows in the matrix.
 * @param col The number of columns in the matrix.
 * @param start The minimum value (inclusive) of the random range.
 * @param end The maximum value (exclusive) of the random range.
 * 
 * @return A pointer to the newly created matrix filled with random values, 
 * or NULL if the matrix creation fails.
 */
Matrix* matrix_random(size_t row, size_t col, size_t start, size_t end) {
    MATRIX_LOG("[matrix_random] Entering function with row=%zu, col=%zu, start=%zu, end=%zu", row, col, start, end);

    Matrix* matrix = matrix_create(row, col);
    srand(time(NULL));

    if (!matrix) {
        MATRIX_LOG("[matrix_random] Error: creation of Matrix object failed");
        return NULL;
    }
    MATRIX_LOG("[matrix_random] Matrix created with size %zux%zu", row, col);

    for (size_t i = 0; i < matrix_size(matrix); i++) {
        matrix->data[i] = (rand() % end) + start;
    }
    MATRIX_LOG("[matrix_random] Matrix filled with random values in range [%zu, %zu)", start, end);

    return matrix;
}

/**
 * @brief Creates a Walsh matrix of size n.
 *
 * This function generates a Walsh matrix, which is a special kind of square 
 * matrix used in various applications such as signal processing and Hadamard 
 * transforms. The size of the matrix must be a power of 2.
 *
 * @param n The size of the Walsh matrix (must be a power of 2).
 * 
 * @return A pointer to the newly created Walsh matrix, or NULL if the size `n` 
 * is not a power of 2 or if the matrix creation fails.
 */
Matrix* matrix_walsh(size_t n) {
    MATRIX_LOG("[matrix_walsh] Entering function with n = %zu", n);
    if (n & (n - 1)) {
        MATRIX_LOG("[matrix_walsh] Error - 'n' (%zu) is not a power of 2", n);
        return NULL;
    }
    MATRIX_LOG("[matrix_walsh] Creating Walsh matrix of size %zux%zu", n, n);

    Matrix* walshMatrix = matrix_create(n, n);
    if (!walshMatrix) {
        MATRIX_LOG("[matrix_walsh] Error - Memory allocation failed for Walsh matrix of size %zux%zu", n, n);
        return NULL;
    }

    MATRIX_LOG("[matrix_walsh] Generating Walsh matrix recursively");
    generateWalshMatrixRecursively(walshMatrix->data, n, n, 0, 0, 1);
    MATRIX_LOG("[matrix_walsh] Successfully generated Walsh matrix");
    
    return walshMatrix;
}
