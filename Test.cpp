#include <iostream>
#include "Matrix.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

template<typename T>
Matrix<T> read(const char *fileName) {
    // I hate you who invented the deny of STL containers (std::string especially) for this task
    char *fullPath = new char[std::strlen(fileName) + 18 + 1];
    std::strcpy(fullPath, "./../examples/");
    std::strcat(fullPath, fileName);
    std::strcat(fullPath, ".txt");

    std::ifstream file(fullPath);
    delete[] fullPath;

    if (!file.is_open()) {
        throw std::invalid_argument("No such file");
    }

    Matrix<T> matrix = Matrix<T>::deserialize(file);
    file.close();
    return matrix;
}

template<typename T>
Matrix<T> parse(const char *text) {
    std::istringstream stream(text);
    return Matrix<T>::deserialize(stream);
}

/**
 * Test base Matrix operations
 *
 * @author danirod12
 */
int main() {
    // Check determinant logic (matrix was generated with determinant on site)
    // deserialize from file
    assert(read<double>("determinant").getDeterminant() == 135553263157621168.0);

    // Check constant comparing
    assert(read<double>("same") == 100);

    // Check matrix creation using setValue
    Matrix<double> matrix(3, 3);
    matrix.setValue(0, 0, 1);
    matrix.setValue(1, 1, 1);
    matrix.setValue(2, 2, 1);
    // Print matrix
    std::cout << matrix << std::endl;

    // deserialize from string stream
    Matrix<double> matrix2 = parse<double>("3 3\n3 4 5\n8 9 6\n5 6 2");
    std::cout << matrix2 << std::endl;
    assert(matrix2 * matrix == matrix2);
    assert(matrix != matrix2);
    assert(matrix == 1);

    // Check elementary rows change
    std::cout << "Elementary (Change rows 1 and 3):" << std::endl;
    matrix2.changeRows(0, 2);
    std::cout << matrix2 << std::endl;
    assert(matrix2 == parse<double>("3 3\n5 6 2\n8 9 6\n3 4 5"));
    assert((matrix2 + matrix) == parse<double>("3 3\n6 6 2\n8 10 6\n3 4 6"));

    // Check elementary row append with other row
    std::cout << "Elementary (1 row -(*2)-> 2 row):" << std::endl;
    matrix2.addRowToAnother(0, 1, 2);
    std::cout << matrix2 << std::endl;
    assert(matrix2 == parse<double>("3 3\n5 6 2\n18 21 10\n3 4 5"));

    // Check elementary row multiply
    std::cout << "Elementary (3 row *2):" << std::endl;
    matrix2.multiplyRow(2, 2.);
    std::cout << matrix2 << std::endl;
    assert(matrix2 == parse<double>("3 3\n5 6 2\n18 21 10\n6 8 10"));

    // Check matrix * scalar
    Matrix<double> matrix3 = matrix2 * 10;
    assert(matrix3 == parse<double>("3 3\n50 60 20\n180 210 100\n60 80 100"));

    // Check matrix * matrix
    Matrix<double> matrix4 = matrix3 * parse<double>("3 3\n8 2 4\n7823 8923 6662\n72 23 24");
    assert(matrix4 == parse<double>("3 3\n471220 535940 400400\n1651470 1876490 1402140\n633520 716260 535600"));

    // Check invertion
    assert(!parse<double>("2 2\n12 23\n8 12") == parse<double>("2 2\n-0.3 0.575\n0.2 -0.3"));
    assert(!parse<double>("4 4\n1 2 1 1\n1 2 3 1\n1 3 5 2\n2 3 2 3") ==
           parse<double>("4 4\n-1 2.5 -1.5 0.5\n1.5 -1 0.5 -0.5\n-0.5 0.5 0 0\n-0.5 -1 0.5 0.5"));

    // Write to file
    std::ofstream out("./../examples/out.txt");
    out << std::setprecision(20) << matrix4;
    out.close();
}
