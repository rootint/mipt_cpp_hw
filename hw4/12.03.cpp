/*
Реализуйте алгоритм поиска подстроки-палиндрома наибольшей длины. Не используйте полный перебор. Ис-
пользуйте кэширование для уменьшения алгоритмической сложности. Используйте стандартный контейнер
std::vector < bool > для представления линеаризованной таблицы кэширования размера N на N, где N -
количество символов в строке. Используйте стандартное представление std::string_view для оптимизации.
*/

#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

auto longest_palindrome(std::string_view text) -> std::string_view
{
    auto const size = text.size();

    if (size < 2)
    {
        return text;
    }

    std::vector<bool> cache(size * size, false);

    auto at = [size, &cache](std::size_t i, std::size_t j) -> std::vector<bool>::reference
    {
        return cache[i * size + j];
    };

    std::size_t best_start = 0;
    std::size_t best_length = 1;

    for (std::size_t i = 0; i < size; ++i)
    {
        at(i, i) = true;
    }

    for (std::size_t i = 0; i + 1 < size; ++i)
    {
        if (text[i] == text[i + 1])
        {
            at(i, i + 1) = true;

            if (best_length < 2)
            {
                best_start = i;
                best_length = 2;
            }
        }
    }

    for (std::size_t length = 3; length <= size; ++length)
    {
        for (std::size_t i = 0; i + length <= size; ++i)
        {
            auto const j = i + length - 1;

            if (text[i] == text[j] && at(i + 1, j - 1))
            {
                at(i, j) = true;

                if (length > best_length)
                {
                    best_start = i;
                    best_length = length;
                }
            }
        }
    }

    return text.substr(best_start, best_length);
}

int main()
{
    using namespace std::string_view_literals;

    assert(longest_palindrome("babad"sv) == "bab"sv || longest_palindrome("babad"sv) == "aba"sv);
    assert(longest_palindrome("cbbd"sv) == "bb"sv);
    assert(longest_palindrome("a"sv) == "a"sv);
    assert(longest_palindrome(""sv) == ""sv);
    assert(longest_palindrome("forgeeksskeegfor"sv) == "geeksskeeg"sv);
    assert(longest_palindrome("racecar"sv) == "racecar"sv);

    auto const text = std::string("the quick brown fox jumps over the level palindrome refer to it");
    std::cout << "input  : " << text << '\n';
    std::cout << "result : " << longest_palindrome(text) << '\n';

    return 0;
}
