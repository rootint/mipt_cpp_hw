/*
Исследуйте систему автоматического увеличения емкости памяти стандартного контейнера std::vector. Определите
коэффициент умножения емкости памяти контейнера в случае нехватки свободных ячеек памяти. Реализуйте тесты для
отслеживания изменений емкости памяти контейнера в процессе вставки новых элементов. Исследуйте систему
автоматического увеличения емкости памяти стандартного контейнера std::deque. Определите размер страниц
непрерывной памяти, используемых для хранения групп смежных элементов контейнера. Реализуйте тесты для
отслеживания адресов элементов контейнера в процессе вставки новых элементов.
*/

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

struct VectorGrowth
{
    std::size_t size = 0;
    std::size_t capacity = 0;
    double factor = 0.0;
};

auto inspect_vector(std::size_t count)
{
    std::vector<int> values;
    std::vector<VectorGrowth> growths;

    auto previous_capacity = values.capacity();

    for (std::size_t i = 0; i < count; ++i)
    {
        values.push_back(static_cast<int>(i));

        if (values.capacity() != previous_capacity)
        {
            auto factor = previous_capacity == 0
                ? 0.0
                : static_cast<double>(values.capacity()) / previous_capacity;

            growths.push_back({values.size(), values.capacity(), factor});
            previous_capacity = values.capacity();
        }
    }

    return growths;
}

auto estimate_vector_multiplier(std::vector<VectorGrowth> const &growths)
{
    std::map<int, int> frequencies;

    for (auto const &growth : growths)
    {
        if (growth.factor > 0.0)
        {
            auto const rounded = static_cast<int>(growth.factor * 100.0 + 0.5);
            ++frequencies[rounded];
        }
    }

    auto best = frequencies.begin();

    for (auto it = frequencies.begin(); it != frequencies.end(); ++it)
    {
        if (it->second > best->second)
        {
            best = it;
        }
    }

    return best->first / 100.0;
}

struct DequeAddress
{
    std::size_t index = 0;
    std::uintptr_t address = 0;
};

auto inspect_deque(std::size_t count)
{
    std::deque<int> values;
    std::vector<DequeAddress> addresses;

    for (std::size_t i = 0; i < count; ++i)
    {
        values.push_back(static_cast<int>(i));
        addresses.push_back({i, reinterpret_cast<std::uintptr_t>(&values[i])});
    }

    return addresses;
}

auto detect_deque_page_sizes(std::vector<DequeAddress> const &addresses)
{
    std::vector<std::size_t> page_sizes;
    std::size_t current_page = 1;

    for (std::size_t i = 1; i < addresses.size(); ++i)
    {
        auto const delta = addresses[i].address - addresses[i - 1].address;

        if (delta == sizeof(int))
        {
            ++current_page;
        }
        else
        {
            page_sizes.push_back(current_page);
            current_page = 1;
        }
    }

    page_sizes.push_back(current_page);
    return page_sizes;
}

auto most_common_page_size(std::vector<std::size_t> const &page_sizes)
{
    std::map<std::size_t, std::size_t> frequencies;

    for (auto page_size : page_sizes)
    {
        ++frequencies[page_size];
    }

    auto best = frequencies.begin();

    for (auto it = frequencies.begin(); it != frequencies.end(); ++it)
    {
        if (it->second > best->second)
        {
            best = it;
        }
    }

    return best->first;
}

int main()
{
    auto const vector_growths = inspect_vector(256);

    assert(!vector_growths.empty());
    assert(vector_growths.front().capacity >= 1);

    auto const vector_multiplier = estimate_vector_multiplier(vector_growths);
    assert(vector_multiplier >= 1.5);

    std::cout << "vector capacity changes:\n";
    for (auto const &growth : vector_growths)
    {
        std::cout << "size = " << std::setw(3) << growth.size
                  << ", capacity = " << std::setw(3) << growth.capacity;

        if (growth.factor > 0.0)
        {
            std::cout << ", factor = " << std::fixed << std::setprecision(2)
                      << growth.factor;
        }

        std::cout << '\n';
    }

    std::cout << "observed vector growth multiplier ~= "
              << std::fixed << std::setprecision(2)
              << vector_multiplier << "\n\n";

    auto const deque_addresses = inspect_deque(512);
    auto const page_sizes = detect_deque_page_sizes(deque_addresses);
    auto const common_page_size = most_common_page_size(page_sizes);

    assert(!page_sizes.empty());
    assert(common_page_size > 1);

    std::cout << "deque page boundaries:\n";
    for (std::size_t i = 1; i < deque_addresses.size(); ++i)
    {
        auto const delta = deque_addresses[i].address - deque_addresses[i - 1].address;

        if (delta != sizeof(int))
        {
            std::cout << "boundary before index " << deque_addresses[i].index
                      << ", delta = " << delta << '\n';
        }
    }

    std::cout << "observed deque page sizes: ";
    for (auto page_size : page_sizes)
    {
        std::cout << page_size << ' ';
    }
    std::cout << '\n';

    std::cout << "most common deque page size = " << common_page_size << '\n';
    std::cout << "10.01 : ok\n";

    return 0;
}
