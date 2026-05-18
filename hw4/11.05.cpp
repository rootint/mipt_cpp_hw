/*
Продемонстрируйте использование диапазонов и стандартных алгоритмов ranges::replace, ranges::fill,
ranges::unique, ranges::rotate и ranges::sample на простых тестах. Реализуйте алгоритм transform_if
на основе комбинации стандартных алгоритмов ranges::transform и ranges::copy_if. Реализуйте алгорит-
мы вычисления средней абсолютной ошибки MAE и среднеквадратичной ошибки MSE на основе комбинации
стандартных численных алгоритмов. Продемонстрируйте использование диапазонов и стандартных представ-
лений views::filter, views::drop, views::join, views::zip и views::stride на простых тестах. Дора-
ботате Ваше предыдущее решение задачи 09.06 таким образом, чтобы пользователь мог вычислять числа ряда
Фибоначчинаосновепредставления.Используйтестандартныйбазовыйклассstd::ranges::view_interface
для определения интерфейса представления через странно рекурсивный шаблон проектирования. Реализуйте
производный класс Fibonacci для представления алгоритма вычисления чисел ряда Фибоначчи. Реализуйте
класс Iterator для представления однонаправленного итератора как приватный вложенный класс в классе
Fibonacci. Реализуйте в классе Fibonacci публичные функции-члены begin и end для создания итераторов.
*/

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <vector>

template <
    std::ranges::input_range R,
    std::weakly_incrementable O,
    typename Transform,
    typename Predicate>
auto transform_if(R &&range, O output, Transform transform, Predicate predicate)
{
    std::vector<std::ranges::range_value_t<R>> buffer;

    std::ranges::transform(range, std::back_inserter(buffer), transform);

    return std::ranges::copy_if(buffer, output, predicate).out;
}

auto mae(std::vector<double> const &a, std::vector<double> const &b) -> double
{
    assert(a.size() == b.size() && !a.empty());

    auto const sum = std::transform_reduce(
        std::cbegin(a), std::cend(a), std::cbegin(b),
        0.0,
        std::plus<>{},
        [](double x, double y) { return std::fabs(x - y); });

    return sum / static_cast<double>(a.size());
}

auto mse(std::vector<double> const &a, std::vector<double> const &b) -> double
{
    assert(a.size() == b.size() && !a.empty());

    auto const sum = std::transform_reduce(
        std::cbegin(a), std::cend(a), std::cbegin(b),
        0.0,
        std::plus<>{},
        [](double x, double y) { auto const d = x - y; return d * d; });

    return sum / static_cast<double>(a.size());
}

class Fibonacci : public std::ranges::view_interface<Fibonacci>
{
public:
    Fibonacci() = default;

    explicit Fibonacci(std::size_t count) : m_count(count) {}

private:
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = int const *;
        using reference = int const &;

        Iterator() = default;

        Iterator(std::size_t index, std::size_t limit)
            : m_index(index), m_limit(limit) {}

        auto operator*() const -> int { return m_current; }

        auto operator++() -> Iterator &
        {
            auto const next = m_current + m_previous;
            m_previous = m_current;
            m_current = next;
            ++m_index;
            return *this;
        }

        auto operator++(int) -> Iterator
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        friend auto operator==(Iterator const &lhs, Iterator const &rhs) -> bool
        {
            return lhs.m_index == rhs.m_index && lhs.m_limit == rhs.m_limit;
        }

    private:
        int m_previous = 0;
        int m_current = 1;
        std::size_t m_index = 0;
        std::size_t m_limit = 0;
    };

public:
    auto begin() const { return Iterator(0, m_count); }

    auto end() const { return Iterator(m_count, m_count); }

private:
    std::size_t m_count = 0;
};

void demo_algorithms()
{
    std::vector<int> vector_1 = {1, 2, 3, 2, 1};

    std::ranges::replace(vector_1, 2, 0);
    assert((vector_1 == std::vector<int>{1, 0, 3, 0, 1}));

    std::vector<int> vector_2(5);
    std::ranges::fill(vector_2, 7);
    assert((vector_2 == std::vector<int>{7, 7, 7, 7, 7}));

    std::vector<int> vector_3 = {1, 1, 2, 2, 3, 3, 3, 4};
    auto const removed = std::ranges::unique(vector_3);
    vector_3.erase(removed.begin(), removed.end());
    assert((vector_3 == std::vector<int>{1, 2, 3, 4}));

    std::vector<int> vector_4 = {1, 2, 3, 4, 5};
    std::ranges::rotate(vector_4, std::next(std::begin(vector_4), 2));
    assert((vector_4 == std::vector<int>{3, 4, 5, 1, 2}));

    std::vector<int> source = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> sample;
    sample.reserve(3);
    std::ranges::sample(source, std::back_inserter(sample), 3, std::mt19937{42});
    assert(sample.size() == 3);
}

void demo_transform_if()
{
    std::vector<int> input = {1, 2, 3, 4, 5};
    std::vector<int> output;

    transform_if(
        input,
        std::back_inserter(output),
        [](int x) { return x * x; },
        [](int x) { return x % 2 == 0; });

    assert((output == std::vector<int>{4, 16}));
}

void demo_metrics()
{
    std::vector<double> truth = {1.0, 2.0, 3.0};
    std::vector<double> predicted = {1.5, 1.5, 4.0};

    auto const mae_value = mae(truth, predicted);
    auto const mse_value = mse(truth, predicted);

    assert(std::fabs(mae_value - 0.666667) < 1e-4);
    assert(std::fabs(mse_value - 0.5) < 1e-9);
}

void demo_views()
{
    std::vector<int> vector = {1, 2, 3, 4, 5, 6};

    auto filtered = vector | std::views::filter([](int x) { return x % 2 == 0; });
    std::vector<int> filtered_vector(std::ranges::begin(filtered), std::ranges::end(filtered));
    assert((filtered_vector == std::vector<int>{2, 4, 6}));

    auto dropped = vector | std::views::drop(2);
    std::vector<int> dropped_vector(std::ranges::begin(dropped), std::ranges::end(dropped));
    assert((dropped_vector == std::vector<int>{3, 4, 5, 6}));

    std::vector<std::vector<int>> nested = {{1, 2}, {3, 4}, {5}};
    auto joined = nested | std::views::join;
    std::vector<int> joined_vector(std::ranges::begin(joined), std::ranges::end(joined));
    assert((joined_vector == std::vector<int>{1, 2, 3, 4, 5}));

    std::vector<int> other = {10, 20, 30};
    auto zipped = std::views::zip(vector, other);
    std::size_t index = 0;
    for (auto const &[lhs, rhs] : zipped)
    {
        assert(lhs == vector[index] && rhs == other[index]);
        ++index;
    }
    assert(index == other.size());

    auto strided = vector | std::views::stride(2);
    std::vector<int> strided_vector(std::ranges::begin(strided), std::ranges::end(strided));
    assert((strided_vector == std::vector<int>{1, 3, 5}));
}

void demo_fibonacci()
{
    Fibonacci fibonacci(10);

    std::vector<int> values(std::ranges::begin(fibonacci), std::ranges::end(fibonacci));

    assert((values == std::vector<int>{1, 1, 2, 3, 5, 8, 13, 21, 34, 55}));

    auto squared = fibonacci | std::views::transform([](int x) { return x * x; });
    std::vector<int> squared_vector(std::ranges::begin(squared), std::ranges::end(squared));
    assert(squared_vector.front() == 1);
    assert(squared_vector.back() == 55 * 55);
}

int main()
{
    demo_algorithms();
    demo_transform_if();
    demo_metrics();
    demo_views();
    demo_fibonacci();

    return 0;
}
