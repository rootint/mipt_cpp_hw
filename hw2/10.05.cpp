/*
Доработайте пример 10.41 таким образом, чтобы пользователь мог исследовать девять хэш-функций, приведенных в
данной статье. Постройте графики зависимости количества возникающих коллизий от количества хэшируемых строк.
Обоснуйте форму полученных зависимостей. Определите лучшие и худшие хэш-функции данного набора. Выполните
сборку решения с флагами -O3 и -m32 из-за особенностей некоторых хэш-функций.
*/

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <string_view>
#include <vector>

auto byte_value(char symbol)
{
    return static_cast<std::uint32_t>(static_cast<unsigned char>(symbol));
}

auto rs_hash(std::string_view text) -> std::uint32_t
{
    std::uint32_t a = 63689;
    std::uint32_t b = 378551;
    std::uint32_t hash = 0;

    for (auto symbol : text)
    {
        hash = hash * a + byte_value(symbol);
        a *= b;
    }

    return hash;
}

auto js_hash(std::string_view text) -> std::uint32_t
{
    std::uint32_t hash = 1315423911;

    for (auto symbol : text)
    {
        hash ^= ((hash << 5) + byte_value(symbol) + (hash >> 2));
    }

    return hash;
}

auto pjw_hash(std::string_view text) -> std::uint32_t
{
    constexpr std::uint32_t bits_in_unsigned_int = 32;
    constexpr std::uint32_t three_quarters = (bits_in_unsigned_int * 3) / 4;
    constexpr std::uint32_t one_eighth = bits_in_unsigned_int / 8;
    constexpr std::uint32_t high_bits =
        0xFFFFFFFFu << (bits_in_unsigned_int - one_eighth);

    std::uint32_t hash = 0;

    for (auto symbol : text)
    {
        hash = (hash << one_eighth) + byte_value(symbol);

        if (auto const test = hash & high_bits; test != 0)
        {
            hash = (hash ^ (test >> three_quarters)) & (~high_bits);
        }
    }

    return hash;
}

auto elf_hash(std::string_view text) -> std::uint32_t
{
    std::uint32_t hash = 0;

    for (auto symbol : text)
    {
        hash = (hash << 4) + byte_value(symbol);

        if (auto const x = hash & 0xF0000000u; x != 0)
        {
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }

    return hash;
}

auto bkdr_hash(std::string_view text) -> std::uint32_t
{
    constexpr std::uint32_t seed = 131;
    std::uint32_t hash = 0;

    for (auto symbol : text)
    {
        hash = hash * seed + byte_value(symbol);
    }

    return hash;
}

auto sdbm_hash(std::string_view text) -> std::uint32_t
{
    std::uint32_t hash = 0;

    for (auto symbol : text)
    {
        hash = byte_value(symbol) + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

auto djb_hash(std::string_view text) -> std::uint32_t
{
    std::uint32_t hash = 5381;

    for (auto symbol : text)
    {
        hash = ((hash << 5) + hash) + byte_value(symbol);
    }

    return hash;
}

auto dek_hash(std::string_view text) -> std::uint32_t
{
    std::uint32_t hash = static_cast<std::uint32_t>(text.size());

    for (auto symbol : text)
    {
        hash = ((hash << 5) ^ (hash >> 27)) ^ byte_value(symbol);
    }

    return hash;
}

auto ap_hash(std::string_view text) -> std::uint32_t
{
    std::uint32_t hash = 0xAAAAAAAAu;

    for (std::size_t i = 0; i < text.size(); ++i)
    {
        auto const value = byte_value(text[i]);

        if ((i & 1u) == 0)
        {
            hash ^= ((hash << 7) ^ (value * (hash >> 3)));
        }
        else
        {
            hash ^= (~((hash << 11) + (value ^ (hash >> 5))));
        }
    }

    return hash;
}

struct HashFunction
{
    std::string_view name;
    std::uint32_t (*function)(std::string_view);
};

struct Result
{
    std::string_view name;
    std::vector<std::size_t> collisions;
    std::size_t total = 0;
};

auto make_strings(std::size_t count)
{
    std::minstd_rand engine(42);
    std::uniform_int_distribution<int> length_distribution(6, 18);
    std::uniform_int_distribution<int> character_distribution(0, 25);

    std::vector<std::string> strings;
    strings.reserve(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        auto text = std::string("key_" + std::to_string(i) + "_");
        auto const target_length = static_cast<std::size_t>(length_distribution(engine));

        while (text.size() < target_length)
        {
            text.push_back(static_cast<char>('a' + character_distribution(engine)));
        }

        strings.push_back(std::move(text));
    }

    return strings;
}

auto count_collisions(
    HashFunction const &hash_function,
    std::vector<std::string> const &strings,
    std::size_t used,
    std::size_t bucket_count)
{
    std::vector<std::size_t> buckets(bucket_count, 0);

    for (std::size_t i = 0; i < used; ++i)
    {
        auto const hash = hash_function.function(strings[i]);
        ++buckets[hash % bucket_count];
    }

    std::size_t collisions = 0;

    for (auto bucket : buckets)
    {
        if (bucket > 1)
        {
            collisions += bucket - 1;
        }
    }

    return collisions;
}

auto make_results(
    std::vector<HashFunction> const &functions,
    std::vector<std::string> const &strings,
    std::vector<std::size_t> const &sample_sizes,
    std::size_t bucket_count)
{
    std::vector<Result> results;

    for (auto const &hash_function : functions)
    {
        Result result{hash_function.name};

        for (auto sample_size : sample_sizes)
        {
            auto const collisions =
                count_collisions(hash_function, strings, sample_size, bucket_count);

            result.collisions.push_back(collisions);
            result.total += collisions;
        }

        results.push_back(std::move(result));
    }

    return results;
}

void print_graphs(
    std::vector<Result> const &results,
    std::vector<std::size_t> const &sample_sizes)
{
    std::size_t max_collision = 0;

    for (auto const &result : results)
    {
        for (auto collision : result.collisions)
        {
            max_collision = std::max(max_collision, collision);
        }
    }

    constexpr std::size_t width = 40;

    for (auto const &result : results)
    {
        std::cout << result.name << '\n';

        for (std::size_t i = 0; i < sample_sizes.size(); ++i)
        {
            auto const collisions = result.collisions[i];
            auto const bar =
                max_collision == 0
                    ? std::size_t{0}
                    : collisions * width / max_collision;

            std::cout << "  n = " << std::setw(5) << sample_sizes[i]
                      << " collisions = " << std::setw(5) << collisions
                      << " | " << std::string(bar, '#') << '\n';
        }

        std::cout << '\n';
    }
}

int main()
{
    auto const functions = std::vector<HashFunction>{
        {"RS", rs_hash},
        {"JS", js_hash},
        {"PJW", pjw_hash},
        {"ELF", elf_hash},
        {"BKDR", bkdr_hash},
        {"SDBM", sdbm_hash},
        {"DJB", djb_hash},
        {"DEK", dek_hash},
        {"AP", ap_hash},
    };

    auto const sample_sizes = std::vector<std::size_t>{256, 512, 1024, 2048, 4096, 8192};
    std::size_t const bucket_count = 4096;
    auto const strings = make_strings(sample_sizes.back());
    auto const results = make_results(functions, strings, sample_sizes, bucket_count);

    for (auto const &result : results)
    {
        assert(result.collisions.size() == sample_sizes.size());

        for (std::size_t i = 1; i < result.collisions.size(); ++i)
        {
            assert(result.collisions[i] >= result.collisions[i - 1]);
        }
    }

    print_graphs(results, sample_sizes);

    auto best = results.begin();
    auto worst = results.begin();

    for (auto it = results.begin(); it != results.end(); ++it)
    {
        if (it->total < best->total)
        {
            best = it;
        }

        if (it->total > worst->total)
        {
            worst = it;
        }
    }

    std::cout << "best hash  = " << best->name
              << " with total collisions = " << best->total << '\n';
    std::cout << "worst hash = " << worst->name
              << " with total collisions = " << worst->total << '\n';
    std::cout
        << "shape explanation: collisions grow monotonically as the number of hashed "
           "strings approaches and exceeds the fixed bucket count. Better hash functions "
           "spread values more evenly, so their curves rise more slowly; worse ones "
           "cluster values early and accumulate collisions faster.\n";
    std::cout << "10.05 : ok\n";

    return 0;
}
