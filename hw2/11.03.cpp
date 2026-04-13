/*
Доработайте Ваше предыдущее решение задачи 09.04 таким образом, чтобы пользователь мог передавать собственный
компаратор в алгоритм сортировки для сравнения элементов. Используйте шаблоны функций. Продемонстрируйте передачу
в алгоритм сортировки пользовательской свободной функцией, стандартного функционального объекта std::less и
пользовательской лямбда-функции в роли собственных компараторов.
*/

#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template <std::random_access_iterator Iterator, typename Compare>
void order(Iterator first, Iterator last, Compare compare)
{
    if (first == last)
    {
        return;
    }

    for (auto current = std::next(first); current != last; ++current)
    {
        for (auto position = current;
             position != first && compare(*position, *std::prev(position));
             --position)
        {
            std::iter_swap(std::prev(position), position);
        }
    }
}

template <std::random_access_iterator Iterator, typename Compare>
auto partition(Iterator first, Iterator last, Compare compare) -> Iterator
{
    auto middle = first;
    std::advance(middle, std::distance(first, last) / 2);

    auto tail = std::prev(last);

    if (compare(*middle, *first))
    {
        std::iter_swap(first, middle);
    }

    if (compare(*tail, *first))
    {
        std::iter_swap(first, tail);
    }

    if (compare(*tail, *middle))
    {
        std::iter_swap(middle, tail);
    }

    auto const pivot = *middle;

    auto left = first;
    auto right = tail;

    while (true)
    {
        while (compare(*left, pivot))
        {
            ++left;
        }

        while (compare(pivot, *right))
        {
            --right;
        }

        if (left >= right)
        {
            return std::next(right);
        }

        std::iter_swap(left, right);
        ++left;
        --right;
    }
}

template <std::random_access_iterator Iterator, typename Compare>
void split(Iterator first, Iterator last, Compare compare)
{
    auto const size = std::distance(first, last);

    if (size <= 1)
    {
        return;
    }

    if (size <= 16)
    {
        order(first, last, compare);
        return;
    }

    auto middle = partition(first, last, compare);

    split(first, middle, compare);
    split(middle, last, compare);
}

template <std::random_access_iterator Iterator, typename Compare = std::less<>>
void sort(Iterator first, Iterator last, Compare compare = Compare())
{
    split(first, last, compare);
}

auto ascending(int lhs, int rhs)
{
    return lhs < rhs;
}

int main()
{
    std::vector<int> numbers = {5, 3, 1, 4, 2, 9, 7, 8, 6, 0};
    sort(numbers.begin(), numbers.end(), ascending);
    assert(std::ranges::is_sorted(numbers, ascending));

    std::array<std::string, 5> words = {
        "delta", "alpha", "echo", "bravo", "charlie"};
    sort(words.begin(), words.end(), std::less<>());
    assert(std::ranges::is_sorted(words, std::less<>()));

    std::deque<int> descending = {1, 3, 5, 2, 4, 6, 7, 0};
    auto greater = [](int lhs, int rhs)
    {
        return lhs > rhs;
    };

    sort(descending.begin(), descending.end(), greater);
    assert(std::ranges::is_sorted(descending, greater));

    std::vector<int> partial = {9, 8, 7, 6, 5, 4, 3};
    sort(std::next(partial.begin()), std::prev(partial.end()), ascending);
    assert(std::ranges::is_sorted(
        std::next(partial.begin()),
        std::prev(partial.end()),
        ascending));

    std::cout << "11.03 : ok\n";

    return 0;
}
