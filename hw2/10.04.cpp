/*
Реализуйте алгоритм вычисления N-ого числа ряда Фибоначчи на основе метода матричной экспоненциации.
Используйте библиотеку Boost.UBLAS для реализации матричных вычислений. Используйте тип usigned long long int
для значений элементов матриц. Реализуйте алгоритм быстрого возведения начальной матрицы в степень N.
Обоснуйте алгоритмическую сложность реализованного алгоритма и сравните ее с алгоритмической сложностью всех
других известных Вам алгоритмов вычисления N-ого числа ряда Фибоначчи.
*/

#include <cassert>
#include <cstddef>
#include <iostream>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/operation.hpp>

using Matrix = boost::numeric::ublas::matrix<unsigned long long int>;

auto identity_matrix()
{
    Matrix matrix(2, 2);
    matrix(0, 0) = 1;
    matrix(0, 1) = 0;
    matrix(1, 0) = 0;
    matrix(1, 1) = 1;
    return matrix;
}

auto fibonacci_matrix()
{
    Matrix matrix(2, 2);
    matrix(0, 0) = 1;
    matrix(0, 1) = 1;
    matrix(1, 0) = 1;
    matrix(1, 1) = 0;
    return matrix;
}

auto power(Matrix base, std::size_t exponent)
{
    auto result = identity_matrix();

    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        {
            result = boost::numeric::ublas::prod(result, base);
        }

        base = boost::numeric::ublas::prod(base, base);
        exponent /= 2;
    }

    return result;
}

auto fibonacci(std::size_t n)
{
    if (n == 0)
    {
        return 0ULL;
    }

    auto const matrix = power(fibonacci_matrix(), n - 1);
    return matrix(0, 0);
}

int main()
{
    assert(fibonacci(0) == 0ULL);
    assert(fibonacci(1) == 1ULL);
    assert(fibonacci(2) == 1ULL);
    assert(fibonacci(10) == 55ULL);
    assert(fibonacci(20) == 6765ULL);
    assert(fibonacci(50) == 12586269025ULL);
    assert(fibonacci(93) == 12200160415121876738ULL);

    for (auto n : {std::size_t{0},
                   std::size_t{1},
                   std::size_t{2},
                   std::size_t{3},
                   std::size_t{5},
                   std::size_t{10},
                   std::size_t{20},
                   std::size_t{50},
                   std::size_t{93}})
    {
        std::cout << "F(" << n << ") = " << fibonacci(n) << '\n';
    }

    std::cout
        << "matrix exponentiation complexity: O(log N) matrix multiplications for a "
           "constant-size 2x2 matrix, so overall O(log N) time.\n";
    std::cout
        << "comparison: naive recursion is exponential, iterative dynamic programming "
           "is O(N), fast doubling is also O(log N), and closed-form floating-point "
           "formulas are O(1) but numerically unreliable for large N.\n";
    std::cout << "10.04 : ok\n";

    return 0;
}
