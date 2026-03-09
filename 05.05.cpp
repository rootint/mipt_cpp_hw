/*
Доработайте Ваше предыдущее решение задачи 04.04 таким образом, чтобы реализация использовала мик-
сины для подмешивания некоторых дополнительных операторов вместо определения всех операторов непо-
средственно внутри дроби. Реализуйте шаблоны базовых классов addable, subtractable, multipliable,
dividable, incrementable и decrementable для представления подмешиваемых дружественных для указан-
ных базовых классов операторов сложения, вычитания, умножения, деления, префиксного и постфиксного
инкремента и декремента дробей соответственно. Реализуйте производный класс Rational, который является
наследником реализации указанных базовых классов. Используйте документацию библиотеки Boost.Operators.
*/
///////////////////////////////////////////////////////////////////////////////////////////////

// chapter : Object-Oriented Programming

///////////////////////////////////////////////////////////////////////////////////////////////

// section : Operator Overloading

///////////////////////////////////////////////////////////////////////////////////////////////

// content : Rational Arithmetic
//
// content : User-Defined Type Conversions
//
// content : Function Specifier explicit
//
// content : Functions std::gcd and std::lcm
//
// content : Operator Overloading
//
// content : Operator double
//
// content : Arithmetic Operators with Assignment
//
// content : Postfix and Prefix Operators
//
// content : Arithmetic and Comparison Operators
//
// content : Operators >> and <<
//
// content : Streams std::istream and std::ostream
//
// content : Stream std::stringstream
//
// content : Three-Way Comparison Operator <=>
//
// content : Expression Rewriting
//
// content : Type std::strong_ordering
//
// content : Class Template
//
// content : Mixins
//
// content : Curiously Recurring Template Pattern
//
// content : Library Boost.Operators

///////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cmath>
#include <compare>
#include <istream>
#include <numeric>
#include <ostream>
#include <sstream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename D>
struct addable
{
    friend auto operator+(D lhs, D const &rhs) { return lhs += rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename D>
struct subtractable
{
    friend auto operator-(D lhs, D const &rhs) { return lhs -= rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename D>
struct multipliable
{
    friend auto operator*(D lhs, D const &rhs) { return lhs *= rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename D>
struct dividable
{
    friend auto operator/(D lhs, D const &rhs) { return lhs /= rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename D>
struct incrementable
{
    friend auto const operator++(D &x, int)
    {
        auto y = x;
        x += D(1);
        return y;
    }

    friend auto &operator++(D &x)
    {
        x += D(1);
        return x;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename D>
struct decrementable
{
    friend auto const operator--(D &x, int)
    {
        auto y = x;
        x -= D(1);
        return y;
    }

    friend auto &operator--(D &x)
    {
        x -= D(1);
        return x;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Rational

    : public addable<Rational<T>>,

      public subtractable<Rational<T>>,

      public multipliable<Rational<T>>,

      public dividable<Rational<T>>,

      public incrementable<Rational<T>>,

      public decrementable<Rational<T>>
{
public:
    /* explicit */ Rational(T num = 0, T den = 1) : m_num(num), m_den(den)
    {
        reduce();
    }

    //  -------------------------------------------------------------------------------------------

    explicit operator double() const
    {
        return 1.0 * m_num / m_den;
    }

    //  -------------------------------------------------------------------------------------------

    auto &operator+=(Rational const &other)
    {
        auto lcm = std::lcm(m_den, other.m_den);

        m_num = m_num * (lcm / m_den) + other.m_num * (lcm / other.m_den);

        m_den = lcm;

        reduce();

        return *this;
    }

    //  -------------------------------------------------------------------------------------------

    auto &operator-=(Rational const &other)
    {
        return *this += Rational(other.m_num * -1, other.m_den);
    }

    //  -------------------------------------------------------------------------------------------

    auto &operator*=(Rational const &other)
    {
        m_num *= other.m_num;

        m_den *= other.m_den;

        reduce();

        return *this;
    }

    //  -------------------------------------------------------------------------------------------

    auto &operator/=(Rational const &other)
    {
        return *this *= Rational(other.m_den, other.m_num);
    }

    //  -------------------------------------------------------------------------------------------

    friend std::strong_ordering operator<=>(Rational const &lhs, Rational const &rhs)
    {
        auto left = lhs.m_num * rhs.m_den;

        auto right = rhs.m_num * lhs.m_den;

        return left <=> right;
    }

    //  -------------------------------------------------------------------------------------------

    friend bool operator==(Rational const &lhs, Rational const &rhs)
    {
        return lhs.m_num == rhs.m_num && lhs.m_den == rhs.m_den;
    }

    //  -------------------------------------------------------------------------------------------

    friend auto &operator>>(std::istream &stream, Rational &rational)
    {
        return (stream >> rational.m_num).ignore() >> rational.m_den;
    }

    //  -------------------------------------------------------------------------------------------

    friend auto &operator<<(std::ostream &stream, Rational const &rational)
    {
        return stream << rational.m_num << '/' << rational.m_den;
    }

private:
    void reduce()
    {
        if (m_den < 0)
        {
            m_num = -m_num;

            m_den = -m_den;
        }

        auto gcd = std::gcd(m_num, m_den);

        m_num /= gcd;

        m_den /= gcd;
    }

    //  -------------------------------------------------------------------------------------------

    T m_num = 0, m_den = 1;
};

///////////////////////////////////////////////////////////////////////////////////////////////

auto equal(double x, double y, double epsilon = 1e-6)
{
    return std::abs(x - y) < epsilon;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    Rational<int> x = 1, y(2, 1);

    //  -----------------------------------------------------------------------

    //	std::vector < int > vector_1 = 5; // error

    std::vector<int> vector_2(5);

    std::vector<int> vector_3 = {1, 2, 3, 4, 5};

    //  -----------------------------------------------------------------------

    assert(equal(static_cast<double>(x), 1));

    //  -----------------------------------------------------------------------

    //	assert(x.operator+=(y) == Rational < int > (3, 1)); // bad

    //  -----------------------------------------------------------------------

    assert((x += y) == Rational<int>(+3, 1));

    assert((x -= y) == Rational<int>(+1, 1));

    assert((x *= y) == Rational<int>(+2, 1));

    assert((x /= y) == Rational<int>(+1, 1));

    //  -----------------------------------------------------------------------

    assert((x++) == Rational<int>(+1, 1));

    assert((x--) == Rational<int>(+2, 1));

    assert((++y) == Rational<int>(+3, 1));

    assert((--y) == Rational<int>(+2, 1));

    //  -----------------------------------------------------------------------

    //	x++++; // error

    //  -----------------------------------------------------------------------

    [[maybe_unused]] auto z = 0;

    //  -----------------------------------------------------------------------

    //	z++++; // error

    //  -----------------------------------------------------------------------

    //	assert(operator+(x, y) == Rational < int > (3, 1)); // bad

    //  -----------------------------------------------------------------------

    assert((x + y) == Rational<int>(+3, 1));

    assert((x - y) == Rational<int>(-1, 1));

    assert((x * y) == Rational<int>(+2, 1));

    assert((x / y) == Rational<int>(+1, 2));

    //  -----------------------------------------------------------------------

    assert((x += 1) == Rational<int>(+2, 1));

    //	assert((1 += x) == Rational < int > (+3, 1)); // error

    assert((x + 1) == Rational<int>(+3, 1));

    assert((1 + y) == Rational<int>(+3, 1));

    assert((1 + 1) == Rational<int>(+2, 1));

    //  -----------------------------------------------------------------------

    assert((x < y) == 0);

    assert((x > y) == 0);

    assert((x <= y) == 1);

    assert((x >= y) == 1);

    assert((x == y) == 1);

    assert((x != y) == 0);

    //  -----------------------------------------------------------------------

    assert((x <=> y) == std::strong_ordering::equal);

    assert((x <=> 1) == std::strong_ordering::greater);

    assert((1 <=> y) == std::strong_ordering::less);

    //  -----------------------------------------------------------------------

    std::stringstream stream_1("1/2");

    std::stringstream stream_2;

    //  -----------------------------------------------------------------------

    stream_1 >> x;

    stream_2 << x;

    //  -----------------------------------------------------------------------

    assert(stream_2.str() == stream_1.str());

    //  -----------------------------------------------------------------------

    Rational<long> a(1L, 3L), b(2L, 3L);

    assert((a + b) == Rational<long>(1L, 1L));
}

///////////////////////////////////////////////////////////////////////////////////////////////