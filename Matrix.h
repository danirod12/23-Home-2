#ifndef INC_23_HOME_1_MATRIX_H
#define INC_23_HOME_1_MATRIX_H

#include <iostream>

class Matrix {
private:
    int columns;
    long size;

    // We store lines each after each. l - line, c - column [l1c1, l1c2, /* new line elements next */ l2c1, l2c2]
    double *array;

    void requireSameDimensions(const Matrix &another) const;

    void requireRow(int row) const;

    void requireColumn(int column) const;

public:
    Matrix(int rows, int columns);

    explicit Matrix(std::istream &stream);

    ~Matrix();

    static Matrix deserialize(std::istream &stream);

    static void serialize(std::ostream &stream, const Matrix &matrix);

    friend std::ostream &operator<<(std::ostream &os, const Matrix &obj);

    [[nodiscard]] int getRows() const;

    [[nodiscard]] int getColumns() const;

    [[nodiscard]] int getSize() const;

    [[nodiscard]] int getInternalIndex(int row, int column) const;

    /**
     * Force-insert value to matrix based on index. Index could be calculated by Matrix::getInternalIndex
     *
     * @param index Internal index calculated by Matrix::getInternalIndex
     * @param value The value
     */
    void setValue(int index, double value);

    void setValue(int row, int column, double value);

    /**
     * Force-get value from matrix based on index. Index could be calculated by Matrix::getInternalIndex
     *
     * @param index Internal index calculated by Matrix::getInternalIndex
     * @param value The value
     */
    [[nodiscard]] double getValue(int index) const;

    [[nodiscard]] double getValue(int row, int column) const;

    /**
     * Get a matrix copy for further changes
     * @return A matrix with same dimensions and same elements
     */
    [[nodiscard]] Matrix clone() const;

    /**
     * Get a matrix minor for element
     * @param row Removing row
     * @param column Removing column
     * @return
     */
    [[nodiscard]] Matrix getMinor(int row, int column) const;

    /**
     * Get determinate for cube matrix (Matrix::isSquare)
     * @return double determinant value
     * @throws invalid_argument if matrix is not cube
     */
    double getDeterminant() const;

    /**
     * Get inverted matrix for current matrix
     * @return Matrix inverted ( Matrix^-1 )
     * @throws invalid_argument if matrix determinant is zero
     */
    Matrix getInverted() const;

    bool isSquare() const;

    Matrix operator+(const Matrix &another) const;

    Matrix operator-() const;

    Matrix operator-(const Matrix &another) const;

    bool operator==(const Matrix &another) const;

    bool operator!=(const Matrix &another) const;

    bool operator==(const double &number) const;

    bool operator!=(const double &number) const;

    Matrix operator*(const Matrix &another) const;

    Matrix operator*(const double &another) const;

    Matrix operator/(const double &another) const;

    Matrix operator!() const;

    /**
     * Change rows for current matrix
     * @param originRow first row
     * @param targetRow second row
     */
    void mutate(int originRow, int targetRow);

    /**
     * Add one row to other multiplied by a value for current matrix
     * @param originRow Row to be added
     * @param targetRow Row where we add first row
     * @param multiplier The multiplier
     */
    void mutate(int originRow, int targetRow, double multiplier);

    /**
     * Multiply row in current matrix
     * @param row Row to be multiplier
     * @param multiplier The multiplier
     */
    void mutate(int row, double multiplier);

    [[nodiscard]] Matrix transposed() const;
};

#endif //INC_23_HOME_1_MATRIX_H
