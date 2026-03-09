/*
Доработайте Ваше предыдущее решение задачи 05.05 таким образом, чтобы пользователь мог обрабатывать
ошибки с помощью исключений. Реализуйте класс Exception для представления пользовательского исключе-
ния, который является наследником интерфейса стандартного исключения std::exception. Переопределите
наследуемую виртуальную функцию-член what в классе Exception. Генерируйте исключение типа Exception
в конструкторе класса Rational при передаче нулевого знаменателя. Реализуйте в функции main блок try для
перехвата исключений и обработчик catch для стандартного исключения std::exception, а также обработ-
чик catch для всех исключений. Используйте стандартный поток std::cerr для вывода через терминал всех
сообщений об ошибках. Продемонстрируйте в функции main генерацию, перехват и обработку стандартных
исключений std::bad_alloc, std::bad_variant_access и std::bad_optional_access, а также стандарт-
ных исключений std::length_error и std::out_of_range, генерируемых функциями-членами стандартного
контейнера std::vector. Объясните причины генерации всех перечисленных выше стандартных исключений.
*/
///////////////////////////////////////////////////////////////////////////////////////////////

// chapter : Object-Oriented Programming

///////////////////////////////////////////////////////////////////////////////////////////////

// section : Operator Overloading, Exception Handling

///////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cmath>
#include <compare>
#include <exception>
#include <iostream>
#include <istream>
#include <numeric>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////

//  Пользовательское исключение, наследник std::exception.

class Exception : public std::exception
{
public:

    explicit Exception(std::string message) : m_message(std::move(message)) {}

//  -------------------------------------------------------------------------------------------

//  Переопределение виртуальной функции-члена what.

    const char * what() const noexcept override
    {
        return m_message.c_str();
    }

private:

    std::string m_message;
};

///////////////////////////////////////////////////////////////////////////////////////////////

template < typename D >
struct addable
{
    friend auto operator+(D lhs, D const & rhs) { return lhs += rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template < typename D >
struct subtractable
{
    friend auto operator-(D lhs, D const & rhs) { return lhs -= rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template < typename D >
struct multipliable
{
    friend auto operator*(D lhs, D const & rhs) { return lhs *= rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template < typename D >
struct dividable
{
    friend auto operator/(D lhs, D const & rhs) { return lhs /= rhs; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template < typename D >
struct incrementable
{
    friend auto const operator++(D & x, int)
    {
        auto y = x;
        x += D(1);
        return y;
    }

    friend auto & operator++(D & x)
    {
        x += D(1);
        return x;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template < typename D >
struct decrementable
{
    friend auto const operator--(D & x, int)
    {
        auto y = x;
        x -= D(1);
        return y;
    }

    friend auto & operator--(D & x)
    {
        x -= D(1);
        return x;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template < typename T >
class Rational

    : public    addable < Rational < T > >,

      public subtractable < Rational < T > >,

      public  multipliable < Rational < T > >,

      public    dividable < Rational < T > >,

      public incrementable < Rational < T > >,

      public decrementable < Rational < T > >
{
public:

//  Конструктор генерирует исключение Exception при нулевом знаменателе.

    /* explicit */ Rational(T num = 0, T den = 1) : m_num(num), m_den(den)
    {
        if (m_den == 0)
        {
            throw Exception("Rational: division by zero, denominator is 0");
        }

        reduce();
    }

//  -------------------------------------------------------------------------------------------

    explicit operator double() const
    {
        return 1.0 * m_num / m_den;
    }

//  -------------------------------------------------------------------------------------------

    auto & operator+=(Rational const & other)
    {
        auto lcm = std::lcm(m_den, other.m_den);

        m_num = m_num * (lcm / m_den) + other.m_num * (lcm / other.m_den);

        m_den = lcm;

        reduce();

        return *this;
    }

//  -------------------------------------------------------------------------------------------

    auto & operator-=(Rational const & other)
    {
        return *this += Rational(other.m_num * -1, other.m_den);
    }

//  -------------------------------------------------------------------------------------------

    auto & operator*=(Rational const & other)
    {
        m_num *= other.m_num;

        m_den *= other.m_den;

        reduce();

        return *this;
    }

//  -------------------------------------------------------------------------------------------

    auto & operator/=(Rational const & other)
    {
        return *this *= Rational(other.m_den, other.m_num);
    }

//  -------------------------------------------------------------------------------------------

    friend std::strong_ordering operator<=>(Rational const & lhs, Rational const & rhs)
    {
        auto left  = lhs.m_num * rhs.m_den;

        auto right = rhs.m_num * lhs.m_den;

        return left <=> right;
    }

//  -------------------------------------------------------------------------------------------

    friend bool operator==(Rational const & lhs, Rational const & rhs)
    {
        return lhs.m_num == rhs.m_num && lhs.m_den == rhs.m_den;
    }

//  -------------------------------------------------------------------------------------------

    friend auto & operator>>(std::istream & stream, Rational & rational)
    {
        return (stream >> rational.m_num).ignore() >> rational.m_den;
    }

//  -------------------------------------------------------------------------------------------

    friend auto & operator<<(std::ostream & stream, Rational const & rational)
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
//  ---------------------------------------------------------------------------
//  1. Демонстрация пользовательского исключения Exception (нулевой знаменатель).
//  ---------------------------------------------------------------------------
//
//  Причина: передача нулевого знаменателя в конструктор Rational. Деление на
//  ноль не имеет математического смысла, поэтому конструктор генерирует
//  исключение типа Exception.

    try
    {
        std::cout << "test 1 : Exception (zero denominator)" << std::endl;

        Rational < int > bad(1, 0);
    }
    catch (std::exception const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error : unknown exception" << std::endl;
    }

//  ---------------------------------------------------------------------------
//  2. Демонстрация std::bad_alloc.
//  ---------------------------------------------------------------------------
//
//  Причина: попытка выделения слишком большого объёма памяти оператором new.
//  Операционная система не может предоставить запрошенный объём памяти,
//  и оператор new генерирует исключение std::bad_alloc.

    try
    {
        std::cout << "test 2 : std::bad_alloc" << std::endl;

        auto size = std::size_t(-1);

        [[maybe_unused]] auto * ptr = new int[size];
    }
    catch (std::bad_alloc const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (std::exception const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error : unknown exception" << std::endl;
    }

//  ---------------------------------------------------------------------------
//  3. Демонстрация std::bad_variant_access.
//  ---------------------------------------------------------------------------
//
//  Причина: попытка извлечения значения неактивного типа из std::variant с
//  помощью std::get. Вариант содержит значение типа int, но запрашивается
//  значение типа double, что приводит к генерации std::bad_variant_access.

    try
    {
        std::cout << "test 3 : std::bad_variant_access" << std::endl;

        std::variant < int, double > variant = 42;

        [[maybe_unused]] auto value = std::get < double > (variant);
    }
    catch (std::bad_variant_access const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (std::exception const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error : unknown exception" << std::endl;
    }

//  ---------------------------------------------------------------------------
//  4. Демонстрация std::bad_optional_access.
//  ---------------------------------------------------------------------------
//
//  Причина: попытка извлечения значения из пустого std::optional с помощью
//  функции-члена value. Объект optional не содержит значения (std::nullopt),
//  поэтому вызов value генерирует исключение std::bad_optional_access.

    try
    {
        std::cout << "test 4 : std::bad_optional_access" << std::endl;

        std::optional < int > optional = std::nullopt;

        [[maybe_unused]] auto value = optional.value();
    }
    catch (std::bad_optional_access const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (std::exception const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error : unknown exception" << std::endl;
    }

//  ---------------------------------------------------------------------------
//  5. Демонстрация std::length_error.
//  ---------------------------------------------------------------------------
//
//  Причина: попытка изменения размера std::vector сверх максимально
//  допустимого значения max_size. Контейнер не может хранить больше
//  элементов, чем позволяет его реализация, и функция-член resize
//  генерирует исключение std::length_error.

    try
    {
        std::cout << "test 5 : std::length_error (vector::resize)" << std::endl;

        std::vector < int > vector;

        vector.resize(vector.max_size() + 1);
    }
    catch (std::length_error const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (std::exception const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error : unknown exception" << std::endl;
    }

//  ---------------------------------------------------------------------------
//  6. Демонстрация std::out_of_range.
//  ---------------------------------------------------------------------------
//
//  Причина: попытка обращения к элементу std::vector по индексу, выходящему
//  за пределы допустимого диапазона, с помощью функции-члена at. В отличие
//  от оператора [], функция at выполняет проверку границ и генерирует
//  исключение std::out_of_range при некорректном индексе.

    try
    {
        std::cout << "test 6 : std::out_of_range (vector::at)" << std::endl;

        std::vector < int > vector = { 1, 2, 3, 4, 5 };

        [[maybe_unused]] auto value = vector.at(10);
    }
    catch (std::out_of_range const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (std::exception const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error : unknown exception" << std::endl;
    }

//  ---------------------------------------------------------------------------
//  7. Основные тесты Rational (из предыдущего решения).
//  ---------------------------------------------------------------------------

    try
    {
        Rational < int > x = 1, y(2, 1);

    //  -------------------------------------------------------------------

        std::vector < int > vector_2(5);

        std::vector < int > vector_3 = { 1, 2, 3, 4, 5 };

    //  -------------------------------------------------------------------

        assert(equal(static_cast < double > (x), 1));

    //  -------------------------------------------------------------------

        assert((x += y) == Rational < int > (+3, 1));

        assert((x -= y) == Rational < int > (+1, 1));

        assert((x *= y) == Rational < int > (+2, 1));

        assert((x /= y) == Rational < int > (+1, 1));

    //  -------------------------------------------------------------------

        assert((x++) == Rational < int > (+1, 1));

        assert((x--) == Rational < int > (+2, 1));

        assert((++y) == Rational < int > (+3, 1));

        assert((--y) == Rational < int > (+2, 1));

    //  -------------------------------------------------------------------

        assert((x + y) == Rational < int > (+3, 1));

        assert((x - y) == Rational < int > (-1, 1));

        assert((x * y) == Rational < int > (+2, 1));

        assert((x / y) == Rational < int > (+1, 2));

    //  -------------------------------------------------------------------

        assert((x += 1) == Rational < int > (+2, 1));

        assert((x +  1) == Rational < int > (+3, 1));

        assert((1 +  y) == Rational < int > (+3, 1));

        assert((1 +  1) == Rational < int > (+2, 1));

    //  -------------------------------------------------------------------

        assert((x <  y) == 0);

        assert((x >  y) == 0);

        assert((x <= y) == 1);

        assert((x >= y) == 1);

        assert((x == y) == 1);

        assert((x != y) == 0);

    //  -------------------------------------------------------------------

        assert((x <=> y) == std::strong_ordering::equal  );

        assert((x <=> 1) == std::strong_ordering::greater);

        assert((1 <=> y) == std::strong_ordering::less   );

    //  -------------------------------------------------------------------

        std::stringstream stream_1("1/2");

        std::stringstream stream_2;

        stream_1 >> x;

        stream_2 << x;

        assert(stream_2.str() == stream_1.str());

    //  -------------------------------------------------------------------

        Rational < long > a(1L, 3L), b(2L, 3L);

        assert((a + b) == Rational < long > (1L, 1L));

    //  -------------------------------------------------------------------

        std::cout << "test 7 : all Rational assertions passed" << std::endl;
    }
    catch (std::exception const & exception)
    {
        std::cerr << "error : " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error : unknown exception" << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////