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
    utl_error_func("Failed to compute the transpose", utl_user_defined_data);
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
  if (!matrix || !matrix_is_square(matrix)) {
    utl_error_func("Matrix must be square for LU decomposition", utl_user_defined_data);
    return false;
  }
  size_t n = matrix->rows;
  *L = matrix_create(n, n);
  *U = matrix_create(n, n);
  if (!(*L) || !(*U)) {
    utl_error_func("Memory allocation failed for L or U", utl_user_defined_data);
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
    }
    for (size_t k = i; k < n; k++) {
      if (i == k) {
        matrix_set(*L, i, i, 1.0); 
      } 
      else {
        double sum = 0.0;
        for (size_t j = 0; j < i; j++) {
          sum += matrix_get(*L, k, j) * matrix_get(*U, j, i);
        }
        matrix_set(*L, k, i, (matrix_get(matrix, k, i) - sum) / matrix_get(*U, i, i));
      }
    }
  }
  return true;
}

bool matrix_qr_decomposition(const Matrix* A, Matrix** Q, Matrix** R) {
  if (!A || A->rows < A->cols) {
    utl_error_func("Matrix must have more rows than columns", utl_user_defined_data);
    return false;
  }
  size_t m = A->rows;
  size_t n = A->cols;
  *Q = matrix_create(m, n);
  *R = matrix_create(n, n);
  if (!*Q || !*R) {
    utl_error_func("Memory allocation failed for Q and R", utl_user_defined_data);
    return false;
  }
  double* a_col = (double*)malloc(sizeof(double) * m);
  double* q_col = (double*)malloc(sizeof(double) * m);
  if (!a_col || !q_col) {
    utl_error_func("Memory allocation failed for column vectors", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      a_col[j] = matrix_get(A, j, i);
    }
    for (size_t k = 0; k < i; ++k) {
      for (size_t j = 0; j < m; ++j) {
        q_col[j] = matrix_get(*Q, j, k);
      }
      subtract_projection(a_col, q_col, m);
    }
    normalize_vector(a_col, m);  
    for (size_t j = 0; j < m; ++j) {
      matrix_set(*Q, j, i, a_col[j]);
    }
  }
  for (size_t j = 0; j < n; ++j) {
    for (size_t i = 0; i <= j; ++i) {
      double r_ij = 0.0;
      for (size_t k = 0; k < m; ++k) {
        r_ij += matrix_get(*Q, k, i) * matrix_get(A, k, j);
      }
      matrix_set(*R, i, j, r_ij);
    }
  }
  free(a_col);
  free(q_col);
  return true;
}

Matrix* matrix_pascal(size_t n) {
  Matrix* pascalMatrix = matrix_create(n, n);
  if (!pascalMatrix) {
    utl_error_func("Memory allocation failed for Pascal matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j <= i; j++) {
      double value = binomial_coefficient(i + j, i);
      matrix_set(pascalMatrix, i, j, value);
      matrix_set(pascalMatrix, j, i, value);
    }
  }
  return pascalMatrix;
}

double matrix_frobenius_norm(const Matrix* matrix) {
  double sum = 0.0;
  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      double value = matrix_get(matrix, i, j);
      sum += value * value;
    }
  }
  double frobeniusNorm = sqrt(sum);
  return frobeniusNorm;
}

double matrix_l1_norm(const Matrix* matrix) {
  double maxSum = 0.0;
  for (size_t j = 0; j < matrix->cols; j++) {
    double columnSum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
      columnSum += fabs(matrix_get(matrix, i, j));
    }
    if (columnSum > maxSum) {
      maxSum = columnSum;
    }
  }
  return maxSum;
}

double matrix_infinity_norm(const Matrix* matrix) {
  double maxSum = 0.0;
  for (size_t i = 0; i < matrix->rows; i++) {
    double rowSum = 0.0;
    for (size_t j = 0; j < matrix->cols; j++) {
      rowSum += fabs(matrix_get(matrix, i, j));
    }
    if (rowSum > maxSum) {
      maxSum = rowSum;
    }
  }
  return maxSum;
}

Matrix* matrix_inverse_gauss_jordan(const Matrix* matrix) {
  if (matrix->rows != matrix->cols) {
    utl_error_func("Matrix must be square for inversion", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->rows;
  Matrix* augmented = matrix_create(n, 2 * n);
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
        matrix_deallocate(augmented);
        return NULL;
      }
      matrix_swap_rows(augmented, col, swapRow);
    }
    matrix_row_divide(augmented, col, augmented->data[col * 2 * n + col]);
    for (size_t row = 0; row < n; row++) {
      if (row != col) {
        matrix_row_subtract(augmented, row, col, augmented->data[row * 2 * n + col]);
      }
    }
  }
  Matrix* inverse = matrix_create(n, n);
  if (!inverse) {
    utl_error_func("Memory allocation failed for inverse matrix", utl_user_defined_data);
    matrix_deallocate(augmented);
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      inverse->data[i * n + j] = augmented->data[i * 2 * n + j + n];
    }
  }
  matrix_deallocate(augmented);
  return inverse;
}

bool matrix_is_positive_definite(const Matrix* matrix) {
  if (!matrix || matrix->rows != matrix->cols) {
    utl_error_func("Matrix must be square to check if it's positive definite", utl_user_defined_data);
    return false;
  }
  if (!matrix_is_symmetric(matrix)) {
    utl_error_func("Matrix is not symmetric", utl_user_defined_data);
    return false;
  }
  Matrix* chol = matrix_cholesky_decomposition(matrix);
  bool isPositiveDefinite = chol != NULL;
  if (isPositiveDefinite) {
    matrix_deallocate(chol); 
  }
  else {
    utl_error_func("Cholesky decomposition failed", utl_user_defined_data);
    return false;
  }
  return isPositiveDefinite;
}

Matrix* matrix_projection(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Input matrix is null", utl_user_defined_data);
    return NULL;
  }
  Matrix* matrixTranspose = matrix_transpose(matrix);
  if (!matrixTranspose) {
    utl_error_func("Transpose calculation failed", utl_user_defined_data);
    return NULL;
  }
  Matrix* mta = matrix_multiply(matrixTranspose, matrix);
  if (!mta) {
    utl_error_func("Multiplication matrix^T * matrix failed", utl_user_defined_data);
    matrix_deallocate(matrixTranspose);
    return NULL;
  }
  Matrix* mtaInv = matrix_inverse(mta);
  if (!mtaInv) {
    utl_error_func("Inverse calculation failed", utl_user_defined_data);
    matrix_deallocate(matrixTranspose); 
    matrix_deallocate(mta); 
    return NULL;
  }
  Matrix* m_mta_inv = matrix_multiply(matrix, mtaInv);
  if (!m_mta_inv) {
    utl_error_func("Multiplication A * (A^T * A)^-1 failed", utl_user_defined_data);
    matrix_deallocate(matrixTranspose);
    matrix_deallocate(mta);
    matrix_deallocate(mtaInv);
    return NULL;
  }
  Matrix* projection = matrix_multiply(m_mta_inv, matrixTranspose);
  if (!projection) {
    utl_error_func("Final projection matrix calculation failed", utl_user_defined_data);
  } 
  matrix_deallocate(matrixTranspose);
  matrix_deallocate(mta);
  matrix_deallocate(mtaInv);
  matrix_deallocate(m_mta_inv);
  return projection;
}

Matrix* matrix_vandermonde(const Matrix* matrix, size_t n) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  Matrix* vandermonde = matrix_create(n, n);
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

Matrix* matrix_companion(const Matrix* coefficients, size_t degree) {
  if (!coefficients) {
    utl_error_func("Coefficients matrix is null", utl_user_defined_data);
    return NULL;
  }
  size_t n = degree - 1; 
  Matrix* companion = matrix_create(n, n);
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

bool matrix_fill(Matrix* matrix, double value) {
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

Matrix* matrix_map(const Matrix* matrix, MatrixFunc func) {
  if (!matrix || !func) {
    utl_error_func("Null argument provided", utl_user_defined_data);
    return NULL;
  }
  Matrix* result = (Matrix*)malloc(sizeof(Matrix));
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

double matrix_min_element(const Matrix* matrix) {
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

double matrix_max_element(const Matrix* matrix) {
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

bool matrix_apply_to_row(Matrix* matrix, size_t row, MatrixFunc func) {
  if (!matrix || !func || row >= matrix->rows) {
    utl_error_func("Invalid arguments, matrix is null or row index out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t j = 0; j < matrix->cols; ++j) {
    matrix->data[row * matrix->cols + j] = func(matrix->data[row * matrix->cols + j]);
  }
  return true;
}

bool matrix_apply_to_col(Matrix* matrix, size_t col, MatrixFunc func) {
  if (!matrix || !func || col >= matrix->cols) {
    utl_error_func("Invalid arguments, matrix is null or column index out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; ++i) {
    matrix->data[i * matrix->cols + col] = func(matrix->data[i * matrix->cols + col]);
  }
  return true;
}

bool matrix_row_addition(Matrix* matrix, size_t targetRow, size_t sourceRow, double scale) {
  if (!matrix || targetRow >= matrix->rows || sourceRow >= matrix->rows) {
    utl_error_func("Invalid arguments, matrix is null or row indices out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t j = 0; j < matrix->cols; ++j) {
    matrix->data[targetRow * matrix->cols + j] += scale * matrix->data[sourceRow * matrix->cols + j];
  }
  return true;
}

bool matrix_col_addition(Matrix* matrix, size_t targetCol, size_t sourceCol, double scale) {
  if (!matrix || targetCol >= matrix->cols || sourceCol >= matrix->cols) {
    utl_error_func("Invalid arguments, matrix is null or column indices out of bounds", utl_user_defined_data);
    return false;
  }
  for (size_t i = 0; i < matrix->rows; ++i) {
    matrix->data[i * matrix->cols + targetCol] += scale * matrix->data[i * matrix->cols + sourceCol];
  }
  return true;
}

Matrix* matrix_leslie(Matrix* f, size_t f_size, Matrix* s, size_t s_size) {
  if (!f) {
    utl_error_func("Matrix f is null", utl_user_defined_data);
    return NULL;
  }
  if (f_size != s_size + 1) {
    utl_error_func("F size must be one more than S size", utl_user_defined_data);
    return NULL;
  }
  Matrix* leslie = matrix_create(f_size, f_size);
  if (!leslie) {
    utl_error_func("Memory allocation failed for Leslie matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < f_size; ++i) {
    matrix_set(leslie, 0, i, f->data[i]);
  }
  for (size_t i = 1; i < f_size; ++i) {
    matrix_set(leslie, i, i - 1, s->data[i - 1]);
  }
  return leslie;
}

Matrix* matrix_fiedler(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null", utl_user_defined_data);
    return NULL;
  }
  size_t n = matrix->cols >= matrix->rows ? matrix->cols : matrix->rows;
  Matrix* fiedler = matrix_create(n, n);
  if (!fiedler) {
    utl_error_func("Memory allocation failed for Fiedler matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      double value = fabs(matrix->data[i] - matrix->data[j]);
      matrix_set(fiedler, i, j, value);
    }
  }
  return fiedler;
}

Matrix* matrix_inverse_hilbert(size_t n) {
  Matrix* invH = matrix_create(n, n);
  if (!invH) {
    utl_error_func("Memory allocation failed for inverse Hilbert matrix", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      int s = i + j;
      int64_t sign = (s % 2 == 0) ? 1 : -1;
      int64_t numerator = sign * (i + j + 1) * binomial_factorial(n + i, n - j - 1) * binomial_factorial(n + j, n - i - 1) * binomial_factorial(s, i) * binomial_factorial(s, j);
      int64_t denominator = 1;
      double value = (double)numerator / denominator;
      matrix_set(invH, i, j, value);
    }
  }
  return invH;
}

Matrix* matrix_get_row(const Matrix* matrix, size_t row) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  else if (row >= matrix->rows) {
    utl_error_func("Row index out of bounds", utl_user_defined_data);
    return NULL;
  }
  Matrix* r = matrix_create(1, matrix->cols);
  if (!r) {
    utl_error_func("Memory allocation failed for row", utl_user_defined_data);
    return NULL;
  }
  for (size_t j = 0; j < matrix->cols; j++) {
    matrix_set(r, 0, j, matrix_get(matrix, row, j));
  }
  return r;
}

Matrix* matrix_get_col(const Matrix* matrix, size_t col) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  else if (col >= matrix->cols) {
    utl_error_func("Column index out of bounds", utl_user_defined_data);
    return NULL;
  }
  Matrix* c = matrix_create(matrix->rows, 1);
  if (!c) {
    utl_error_func("Memory allocation failed for column", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix->rows; i++) {
    matrix_set(c, i, 0, matrix_get(matrix, i, col));
  }
  return c;
}

double* matrix_to_array(const Matrix* matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  size_t size = matrix->rows * matrix->cols * sizeof(double);
  double* data = (double*) malloc(size);
  if (!data) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  memcpy(data, matrix->data, size);
  return data;
}

Matrix* matrix_block_diag(size_t count, ...) {
  va_list args;
  size_t totalRows = 0, totalCols = 0;
  va_start(args, count);
  for (size_t i = 0; i < count; ++i) {
    Matrix* mat = va_arg(args, Matrix*);
    totalRows += mat->rows;
    totalCols += mat->cols;
  }
  va_end(args);
  Matrix* result = matrix_create(totalRows, totalCols);
  if (!result) {
    utl_error_func("Memory allocation failed for block diagonal matrix", utl_user_defined_data);
    return NULL;
  }
  size_t currentRow = 0, currentCol = 0;
  va_start(args, count);
  for (size_t i = 0; i < count; ++i) {
    Matrix* mat = va_arg(args, Matrix*);
    for (size_t r = 0; r < mat->rows; ++r) {
      memcpy(result->data + (currentRow + r) * totalCols + currentCol, mat->data + r * mat->cols, mat->cols * sizeof(double));
    }
    currentRow += mat->rows;
    currentCol += mat->cols;
  }
  va_end(args);
  return result;
}

bool matrix_is_sparse(const Matrix* matrix) {
  if (!matrix || !matrix->data) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return false;
  }
  size_t totalElements = matrix->rows * matrix->cols;
  size_t nonZeroCount = 0;
  for (size_t i = 0; i < totalElements; ++i) {
    if (matrix->data[i] != 0) {
      ++nonZeroCount;
    }
  }
  double nonZeroPercentage = (double)nonZeroCount / (double)totalElements;
  bool isSparse = nonZeroPercentage < 0.3;
  return isSparse;
}

size_t matrix_size(const Matrix *matrix) {
  if (!matrix) {
    utl_error_func("Matrix object is null or invalid", utl_user_defined_data);
    return 0;
  }
  size_t size = matrix->rows * matrix->cols;
  return size;
}

Matrix* matrix_random(size_t row, size_t col, size_t start, size_t end) {
  Matrix* matrix = matrix_create(row, col);
  srand(time(NULL));
  if (!matrix) {
    utl_error_func("Creation of Matrix object failed", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < matrix_size(matrix); i++) {
    matrix->data[i] = (rand() % end) + start;
  }
  return matrix;
}

Matrix* matrix_walsh(size_t n) {
  if (n & (n - 1)) {
    utl_error_func("N is not a power of 2", utl_user_defined_data);
    return NULL;
  }
  Matrix* walshMatrix = matrix_create(n, n);
  if (!walshMatrix) {
    utl_error_func("Memory allocation failed for Walsh matrix", utl_user_defined_data);
    return NULL;
  }
  generateWalshMatrixRecursively(walshMatrix->data, n, n, 0, 0, 1);
  return walshMatrix;
}

