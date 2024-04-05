
#include "Matrix.h"
#include <stdexcept>

// init & destruct

Matrix::Matrix(int rows, int columns) {
    array = (double *) malloc(sizeof(double) * rows * columns);
    this->columns = columns;
    this->size = columns * rows;

    // reset memory after malloc
    for (int i = 0; i < size; ++i) {
        this->array[i] = 0.0;
    }
}

Matrix::Matrix(std::istream &stream) {
    Matrix deserialized = deserialize(stream);
    this->array = deserialized.array;
    this->columns = deserialized.columns;
    this->size = deserialized.size;
}

Matrix::~Matrix() {
    free(array);
}

Matrix Matrix::deserialize(std::istream &stream) {
    int columns, rows;
    stream >> rows;
    stream >> columns;

    Matrix matrix(rows, columns);
    columns *= rows;
    for (int i = 0; i < columns; ++i) {
        stream >> matrix.array[i];
    }
    return matrix;
}

void Matrix::serialize(std::ostream &stream, const Matrix &matrix) {
    for (int i = 0; i < matrix.getSize(); ++i) {
        stream << matrix.getValue(i);
        if ((i + 1) % matrix.getColumns() == 0) {
            stream << std::endl;
        } else {
            stream << "\t";
        }
    }
}

// private methods

void Matrix::requireSameDimensions(const Matrix &another) const {
    if (this->size != another.size || this->columns != another.columns) {
        throw std::invalid_argument("Matrix is not compatible for operation");
    }
}

void Matrix::requireRow(int row) const {
    if (row < 0 || row >= (this->size / this->columns)) {
        throw std::invalid_argument("Matrix out of bounds");
    }
}

void Matrix::requireColumn(int column) const {
    if (column >= this->columns || column < 0) {
        throw std::invalid_argument("Matrix out of bounds");
    }
}

// public methods

int Matrix::getColumns() const {
    return this->columns;
}

int Matrix::getRows() const {
    return this->size / this->columns;
}

int Matrix::getSize() const {
    return this->size;
}

int Matrix::getInternalIndex(int row, int column) const {
    this->requireColumn(column);
    this->requireRow(row);

    return row * this->columns + column;
}

/**
 * Force-insert value to matrix based on index. Index could be calculated by Matrix::getInternalIndex
 *
 * @param index Internal index calculated by Matrix::getInternalIndex
 * @param value The value
 */
void Matrix::setValue(int index, double value) {
    if (index < 0 || index >= this->size) {
        throw std::invalid_argument("Matrix out of bounds");
    }
    this->array[index] = value;
}

void Matrix::setValue(int row, int column, double value) {
    this->array[this->getInternalIndex(row, column)] = value;
}

/**
* Force-get value from matrix based on index. Index could be calculated by Matrix::getInternalIndex
*
* @param index Internal index calculated by Matrix::getInternalIndex
* @param value The value
*/
double Matrix::getValue(int index) const {
    if (index < 0 || index >= this->size) {
        throw std::invalid_argument("Matrix out of bounds");
    }
    return this->array[index];
}

double Matrix::getValue(int row, int column) const {
    return this->array[this->getInternalIndex(row, column)];
}

/**
 * Get a matrix copy for further changes
 * @return A matrix with same dimensions and same elements
 */
Matrix Matrix::clone() const {
    Matrix matrix(this->getRows(), this->columns);
    for (int i = 0; i < this->size; ++i) {
        matrix.array[i] = this->array[i];
    }
    return matrix;
}

/**
 * Get a matrix minor for element
 * @param row Removing row
 * @param column Removing column
 * @return
 */
Matrix Matrix::getMinor(int row, int column) const {
    requireRow(row);
    requireColumn(column);
    if (this->size == this->columns || this->columns == 1) {
        throw std::invalid_argument("Cannot extract minor from matrix of 1 dimension");
    }

    Matrix matrix(this->getRows() - 1, this->columns - 1);
    int jumpIndex = this->columns * row;
    for (int i = 0, j = 0; i < matrix.size; ++i, ++j) {
        // skip row that we do not need
        if (j == jumpIndex) {
            j += this->columns;
        }
        // skip column that we do not need
        if ((j % this->columns) == column) {
            // here we use i-- not j++ because we may want to jump from new row that we got
            // (execute row skip check just here), it could be achieved if we forward j increment
            // to for loop (for loop increment both i and j, so we decrement i)
            i--;
            continue;
        }

        matrix.array[i] = this->array[j];
    }
    return matrix;
}

double Matrix::getDeterminant() const { // NOLINT
    if (!isSquare()) {
        throw std::invalid_argument("Matrix is not cube");
    }

    if (this->size == 1) {
        return this->array[0];
    } else if (this->size == 4) {
        return this->array[0] * this->array[3] - this->array[1] * this->array[2];
    } else {
        double value = 0.0;
        for (int i = 0; i < this->columns; ++i) {
            if (this->array[i] != 0) {
                value += (i % 2 == 0 ? 1 : -1) * this->array[i] * this->getMinor(0, i).getDeterminant();
            }
        }
        return value;
    }
}

Matrix Matrix::getInverted() const {
    double determinant = this->getDeterminant();
    if (determinant == 0) {
        throw std::invalid_argument("Matrix cannot be inverted");
    }

    int rows = this->getRows();
    Matrix additions(rows, this->columns);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < additions.columns; ++j) {
            additions.setValue(i, j, ((i + j) % 2 == 0 ? 1 : -1)
                                     * this->getMinor(i, j).getDeterminant());
        }
    }
    return additions.transposed() / determinant;
}

bool Matrix::isSquare() const {
    return this->columns * this->columns == this->size;
}

Matrix Matrix::operator+(const Matrix &another) const {
    this->requireSameDimensions(another);
    Matrix matrix(this->getRows(), this->columns);
    for (int i = 0; i < this->size; ++i) {
        matrix.array[i] = this->array[i] + another.array[i];
    }
    return matrix;
}

Matrix Matrix::operator-() const {
    return (*this) * -1;
}

Matrix Matrix::operator-(const Matrix &another) const {
    Matrix copy = another.clone();
    return *this + (-copy);
}

Matrix Matrix::operator!() const {
    return this->getInverted();
}

bool Matrix::operator==(const Matrix &another) const {
    if (this->size != another.size || this->columns != another.columns) {
        return false;
    }

    for (int i = 0; i < this->size; ++i) {
        if (another.array[i] != this->array[i]) {
            return false;
        }
    }
    return true;
}

bool Matrix::operator!=(const Matrix &another) const {
    return !(*this == another);
}

bool Matrix::operator==(const double &number) const {
    if (!this->isSquare()) {
        return false;
    }

    for (int i = 0; i < this->size; ++i) {
        if ((i / columns == i % columns ? number : 0) != this->array[i]) {
            return false;
        }
    }
    return true;
}

bool Matrix::operator!=(const double &number) const {
    return !(*this == number);
}

Matrix Matrix::operator*(const Matrix &another) const {
    if (this->columns != another.getRows()) {
        throw std::invalid_argument("Matrix cannot be multiplied (col1 != row2)");
    }

    Matrix matrix(this->getRows(), another.columns);
    for (int i = 0; i < matrix.getSize(); ++i) {
        int column = i % matrix.columns;
        int row = i / matrix.columns;

        double value = 0.0;
        for (int j = 0; j < this->columns; ++j) {
            value += this->getValue(row, j) * another.getValue(j, column);
        }
        matrix.array[i] = value;
    }
    return matrix;
}

Matrix Matrix::operator*(const double &another) const {
    Matrix matrix(this->getRows(), this->columns);
    for (int i = 0; i < this->size; ++i) {
        matrix.array[i] = this->array[i] * another;
    }
    return matrix;
}

Matrix Matrix::operator/(const double &another) const {
    Matrix matrix(this->getRows(), this->columns);
    for (int i = 0; i < this->size; ++i) {
        matrix.array[i] = this->array[i] / another;
    }
    return matrix;
}

std::ostream &operator<<(std::ostream &os, const Matrix &obj) {
    Matrix::serialize(os, obj);
    return os;
}

std::istream &operator>>(std::istream &is, Matrix &obj) {
    obj = Matrix::deserialize(is);
    return is;
}

/**
 * Change rows for current matrix
 * @param originRow first row
 * @param targetRow second row
 */
void Matrix::mutate(int originRow, int targetRow) {
    this->requireRow(originRow);
    this->requireRow(targetRow);

    int lastIndex = this->columns * (originRow + 1);
    int offsetIndex = (targetRow - originRow) * this->columns;
    for (int i = originRow * this->columns; i < lastIndex; ++i) {
        int j = i + offsetIndex;
        double originValue = this->array[j];
        this->array[j] = this->array[i];
        this->array[i] = originValue;
    }
}

/**
 * Add one row to other multiplied by a value for current matrix
 * @param originRow Row to be added
 * @param targetRow Row where we add first row
 * @param multiplier The multiplier
 */
void Matrix::mutate(int originRow, int targetRow, double multiplier) {
    this->requireRow(originRow);
    this->requireRow(targetRow);

    int lastIndex = this->columns * (targetRow + 1);
    int offsetIndex = (originRow - targetRow) * this->columns;
    for (int i = targetRow * this->columns; i < lastIndex; ++i) {
        this->array[i] += multiplier * this->array[i + offsetIndex];
    }
}

/**
 * Multiply row in current matrix
 * @param row Row to be multiplier
 * @param multiplier The multiplier
 */
void Matrix::mutate(int row, double multiplier) {
    this->requireRow(row);

    int lastIndex = this->columns * (row + 1);
    for (int i = row * this->columns; i < lastIndex; ++i) {
        this->array[i] *= multiplier;
    }
}

Matrix Matrix::transposed() const {
    Matrix matrix(this->columns, this->getRows()); // NOLINT
    for (int i = 0; i < this->columns; ++i) {
        for (int j = 0; j < matrix.columns; ++j) {
            matrix.setValue(i, j, this->getValue(j, i));
        }
    }
    return matrix;
}
