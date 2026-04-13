/*
Доработайте Ваше предыдущее решение задачи 04.01 таким образом, чтобы реализация использовала итераторы
вместо индексов. Реализуйте передачу коллекции элементов в алгоритм сортировки по значению двух итераторов
начала и конца коллекции. Используйте полуоткрытые диапазоны. Предполагайте, что элементы коллекции могут
храниться в любом контейнере, который обладает итераторами произвольного доступа. Используйте стандартные
функции std::distance, std::advance, std::next, std::prev и std::iter_swap.
*/

#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template <std::random_access_iterator Iterator>
void order(Iterator first, Iterator last)
{
    if (first == last)
    {
        return;
    }

    for (auto current = std::next(first); current != last; ++current)
    {
        for (auto position = current;
             position != first && *std::prev(position) > *position;
             --position)
        {
            std::iter_swap(std::prev(position), position);
        }
    }
}

template <std::random_access_iterator Iterator>
auto partition(Iterator first, Iterator last) -> Iterator
{
    auto middle = first;
    std::advance(middle, std::distance(first, last) / 2);

    auto tail = std::prev(last);

    if (*first > *middle)
    {
        std::iter_swap(first, middle);
    }

    if (*first > *tail)
    {
        std::iter_swap(first, tail);
    }

    if (*middle > *tail)
    {
        std::iter_swap(middle, tail);
    }

    auto const pivot = *middle;

    auto left = first;
    auto right = tail;

    while (true)
    {
        while (*left < pivot)
        {
            ++left;
        }

        while (*right > pivot)
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

template <std::random_access_iterator Iterator>
void split(Iterator first, Iterator last)
{
    auto const size = std::distance(first, last);

    if (size <= 1)
    {
        return;
    }

    if (size <= 16)
    {
        order(first, last);
        return;
    }

    auto middle = partition(first, last);

    split(first, middle);
    split(middle, last);
}

template <std::random_access_iterator Iterator>
void sort(Iterator first, Iterator last)
{
    split(first, last);
}

int main()
{
    std::vector<int> empty;
    sort(empty.begin(), empty.end());
    assert(std::ranges::is_sorted(empty));

    std::vector<int> values = {5, 3, 1, 4, 2, 9, 7, 8, 6, 0, 3, 3};
    auto values_reference = values;
    std::ranges::sort(values_reference);

    sort(values.begin(), values.end());

    assert(values == values_reference);
    assert(std::ranges::is_sorted(values));

    std::deque<int> deque = {9, 4, 7, 1, 3, 8, 2, 6, 5, 0};
    sort(deque.begin(), deque.end());
    assert(std::ranges::is_sorted(deque));

    std::array<std::string, 5> words = {
        "delta", "alpha", "echo", "bravo", "charlie"};
    sort(words.begin(), words.end());
    assert(std::ranges::is_sorted(words));

    std::vector<int> partial = {9, 8, 7, 6, 5, 4, 3};
    sort(std::next(partial.begin()), std::prev(partial.end()));

    assert(partial.front() == 9);
    assert(partial.back() == 3);
    assert(std::ranges::is_sorted(
        std::next(partial.begin()),
        std::prev(partial.end())));

    std::cout << "09.04 : ok\n";

    return 0;
}
