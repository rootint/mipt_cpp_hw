/*
Реализуйте алгоритм вычисления чисел ряда Фибоначчи на основе однонаправленного итератора. Реализуйте
класс Iterator для представления однонаправленного итератора. Реализуйте в классе Iterator два приватных
поля типа int для хранения двух смежных чисел ряда Фибоначчи. Реализуйте в классе Iterator конструктор по
умолчанию, перегруженные операторы префиксного и постфиксного инкремента, разыменования и сравнения на
равенство. Реализуйте операторы инкремента таким образом, чтобы они вычисляли следующие два смежных числа
ряда Фибоначчи на основе двух текущих смежных чисел ряда Фибоначчи, а оператор разыменования таким
образом, чтобы он возвращал последнее вычисленное число ряда Фибоначчи. Реализуйте альтернативный алгоритм
вычисления чисел ряда Фибоначчи на основе однонаправленного итератора. Используйте библиотеку Boost.Iterator
для реализации интерфейса класса Iterator на основе фасада.
*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <vector>

#include <boost/iterator/iterator_facade.hpp>

class Fibonacci
{
public:
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = int const *;
        using reference = int const &;

        Iterator() = default;

        explicit Iterator(std::size_t position, std::size_t size)
            : m_position(position), m_size(size)
        {
            for (std::size_t i = 0; i < position; ++i)
            {
                advance();
            }
        }

        auto operator++() -> Iterator &
        {
            advance();
            ++m_position;
            return *this;
        }

        auto operator++(int) -> Iterator
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        auto operator*() const -> int
        {
            return m_current;
        }

        friend auto operator==(Iterator const &lhs, Iterator const &rhs)
        {
            return lhs.m_position == rhs.m_position && lhs.m_size == rhs.m_size;
        }

    private:
        void advance()
        {
            auto const next_value = m_current + m_next;
            m_current = m_next;
            m_next = next_value;
        }

        int m_current = 0;
        int m_next = 1;
        std::size_t m_position = 0;
        std::size_t m_size = 0;
    };

    explicit Fibonacci(std::size_t size) : m_size(size) {}

    auto begin() const
    {
        return Iterator(0, m_size);
    }

    auto end() const
    {
        return Iterator(m_size, m_size);
    }

private:
    std::size_t m_size;
};

class Fibonacci_Facade
{
public:
    class Iterator
        : public boost::iterator_facade<
              Iterator,
              int const,
              boost::forward_traversal_tag,
              int>
    {
    public:
        Iterator() = default;

        explicit Iterator(std::size_t position, std::size_t size)
            : m_size(size)
        {
            for (std::size_t i = 0; i < position; ++i)
            {
                increment();
            }
        }

    private:
        friend class boost::iterator_core_access;

        void increment()
        {
            auto const next_value = m_current + m_next;
            m_current = m_next;
            m_next = next_value;
            ++m_position;
        }

        auto equal(Iterator const &other) const
        {
            return m_position == other.m_position && m_size == other.m_size;
        }

        auto dereference() const
        {
            return m_current;
        }

        int m_current = 0;
        int m_next = 1;
        std::size_t m_position = 0;
        std::size_t m_size = 0;
    };

    explicit Fibonacci_Facade(std::size_t size) : m_size(size) {}

    auto begin() const
    {
        return Iterator(0, m_size);
    }

    auto end() const
    {
        return Iterator(m_size, m_size);
    }

private:
    std::size_t m_size;
};

template <typename Range>
auto collect(Range const &range)
{
    std::vector<int> values;

    for (auto value : range)
    {
        values.push_back(value);
    }

    return values;
}

int main()
{
    auto const expected = std::vector<int>{0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

    auto const manual = collect(Fibonacci(expected.size()));
    auto const facade = collect(Fibonacci_Facade(expected.size()));

    assert(manual == expected);
    assert(facade == expected);
    assert(std::ranges::equal(manual, facade));

    for (auto value : manual)
    {
        std::cout << value << ' ';
    }

    std::cout << "\n09.06 : ok\n";

    return 0;
}
