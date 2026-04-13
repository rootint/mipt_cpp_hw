/*
Доработайте пример 09.31 таким образом, чтобы пользователь мог выбирать алгоритм поиска свободных блоков
памяти. Реализуйте в классе Allocator приватную функцию-член find_first для представления алгоритма поиска
первого подходящего свободного блока памяти. Реализуйте в классе Allocator приватную функцию-член find_best
для представления алгоритма поиска лучшего подходящего свободного блока памяти. Реализуйте в конструкторе
класса Allocator дополнительный параметр для выбора алгоритм поиска свободных блоков памяти. Сравните
среднее время работы аллокатора, использующего первый алгоритм поиска свободных блоков памяти, и среднее
время работы того же аллокатора, использующего второй алгоритм поиска, в рамках одинаковых тестов.
Используйте библиотеку Google.Benchmark для реализации бенчмарков.
*/

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <new>
#include <random>
#include <utility>
#include <vector>

#include </usr/include/benchmark/benchmark.h>

class Allocator
{
public:
    enum class Strategy
    {
        first_fit,
        best_fit,
    };

    explicit Allocator(std::size_t size, Strategy strategy = Strategy::first_fit)
        : m_size(size), m_strategy(strategy)
    {
        assert(m_size >= sizeof(Node) + 1);

        m_begin = ::operator new(m_size, std::align_val_t(s_alignment));
        m_head = get_node(m_begin);
        m_head->size = m_size - sizeof(Header);
        m_head->next = nullptr;
    }

    Allocator(Allocator const &) = delete;
    auto operator=(Allocator const &) -> Allocator & = delete;

    ~Allocator()
    {
        ::operator delete(m_begin, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void *
    {
        void *end = get_byte(m_begin) + sizeof(Header) + size;
        void *next = end;

        auto free = 2 * alignof(Header);

        if ((next = std::align(alignof(Header), sizeof(Header), next, free)))
        {
            auto const padding =
                static_cast<std::size_t>(get_byte(next) - get_byte(end));

            auto const [current, previous] = find(size + padding);

            if (!current)
            {
                return nullptr;
            }

            auto actual_padding = padding;

            if (current->size >= size + actual_padding + sizeof(Node) + 1)
            {
                auto const step = sizeof(Header) + size + actual_padding;
                auto *node = get_node(get_byte(current) + step);

                node->size = current->size - step;
                node->next = current->next;

                current->next = node;
            }
            else
            {
                actual_padding += current->size - size - actual_padding;
            }

            if (!previous)
            {
                m_head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            auto *header = get_header(current);
            header->size = size + actual_padding;

            return get_byte(current) + sizeof(Header);
        }

        return nullptr;
    }

    void deallocate(void *pointer)
    {
        if (!pointer)
        {
            return;
        }

        auto *node = get_node(get_byte(pointer) - sizeof(Header));
        auto *header = get_header(node);

        node->size = header->size;
        node->next = nullptr;

        Node *previous = nullptr;
        Node *current = m_head;

        while (current)
        {
            if (node < current)
            {
                node->next = current;

                if (!previous)
                {
                    m_head = node;
                }
                else
                {
                    previous->next = node;
                }

                merge(previous, node);
                return;
            }

            previous = current;
            current = current->next;
        }

        if (!previous)
        {
            m_head = node;
        }
        else
        {
            previous->next = node;
        }

        merge(previous, node);
    }

    void show() const
    {
        std::cout << "Allocator::show : strategy = "
                  << (m_strategy == Strategy::first_fit ? "first_fit" : "best_fit")
                  << ", free blocks = ";

        for (auto *node = m_head; node; node = node->next)
        {
            std::cout << '[' << node->size << "] ";
        }

        std::cout << '\n';
    }

private:
    struct Node
    {
        std::size_t size = 0;
        Node *next = nullptr;
    };

    struct alignas(std::max_align_t) Header
    {
        std::size_t size = 0;
    };

    static_assert(sizeof(Node) == sizeof(Header));

    auto get_byte(void *pointer) const -> std::byte *
    {
        return static_cast<std::byte *>(pointer);
    }

    auto get_byte(void const *pointer) const -> std::byte const *
    {
        return static_cast<std::byte const *>(pointer);
    }

    auto get_node(void *pointer) const -> Node *
    {
        return static_cast<Node *>(pointer);
    }

    auto get_header(void *pointer) const -> Header *
    {
        return static_cast<Header *>(pointer);
    }

    auto find(std::size_t size) const -> std::pair<Node *, Node *>
    {
        if (m_strategy == Strategy::best_fit)
        {
            return find_best(size);
        }

        return find_first(size);
    }

    auto find_first(std::size_t size) const -> std::pair<Node *, Node *>
    {
        auto *current = m_head;
        Node *previous = nullptr;

        while (current && current->size < size)
        {
            previous = current;
            current = current->next;
        }

        return {current, previous};
    }

    auto find_best(std::size_t size) const -> std::pair<Node *, Node *>
    {
        Node *best = nullptr;
        Node *best_previous = nullptr;
        Node *current = m_head;
        Node *previous = nullptr;

        while (current)
        {
            if (current->size >= size &&
                (!best || current->size < best->size))
            {
                best = current;
                best_previous = previous;
            }

            previous = current;
            current = current->next;
        }

        return {best, best_previous};
    }

    void merge(Node *previous, Node *node)
    {
        if (node->next &&
            get_byte(node) + sizeof(Header) + node->size == get_byte(node->next))
        {
            node->size += sizeof(Header) + node->next->size;
            node->next = node->next->next;
        }

        if (previous &&
            get_byte(previous) + sizeof(Header) + previous->size == get_byte(node))
        {
            previous->size += sizeof(Header) + node->size;
            previous->next = node->next;
        }
    }

    std::size_t m_size = 0;
    Strategy m_strategy = Strategy::first_fit;
    void *m_begin = nullptr;
    Node *m_head = nullptr;

    static inline constexpr auto s_alignment = alignof(std::max_align_t);
};

void verify_strategy(Allocator::Strategy strategy)
{
    Allocator allocator(1024, strategy);

    allocator.show();
    allocator.allocate(16);

    allocator.show();
    auto *x = allocator.allocate(16);

    allocator.show();
    auto *y = allocator.allocate(16);

    allocator.show();
    allocator.allocate(16);

    allocator.show();
    allocator.deallocate(y);

    allocator.show();
    allocator.deallocate(x);

    allocator.show();
    auto *z = allocator.allocate(32);

    allocator.show();
    assert(z == x);

    allocator.deallocate(z);
}

static void bench_allocator(benchmark::State &state, Allocator::Strategy strategy)
{
    constexpr std::size_t pool_size = std::size_t{8} << 20;
    constexpr std::size_t slots = 512;

    std::minstd_rand engine(42);
    std::uniform_int_distribution<std::size_t> distribution(32, 1024);

    std::vector<std::size_t> sizes(slots);
    for (auto &size : sizes)
    {
        size = distribution(engine);
    }

    std::vector<void *> blocks(slots, nullptr);

    for (auto _ : state)
    {
        state.PauseTiming();
        Allocator allocator(pool_size, strategy);
        std::fill(blocks.begin(), blocks.end(), nullptr);
        state.ResumeTiming();

        for (std::size_t i = 0; i < slots; ++i)
        {
            blocks[i] = allocator.allocate(sizes[i]);
            benchmark::DoNotOptimize(blocks[i]);
        }

        for (std::size_t i = 0; i < slots; i += 3)
        {
            allocator.deallocate(blocks[i]);
            blocks[i] = nullptr;
        }

        for (std::size_t i = 0; i < slots; i += 3)
        {
            blocks[i] = allocator.allocate(sizes[i] / 2 + 17);
            benchmark::DoNotOptimize(blocks[i]);
        }

        for (auto *pointer : blocks)
        {
            allocator.deallocate(pointer);
        }
    }
}

BENCHMARK_CAPTURE(bench_allocator, first_fit, Allocator::Strategy::first_fit)
    ->Iterations(50);
BENCHMARK_CAPTURE(bench_allocator, best_fit, Allocator::Strategy::best_fit)
    ->Iterations(50);

int main(int argc, char **argv)
{
    verify_strategy(Allocator::Strategy::first_fit);
    verify_strategy(Allocator::Strategy::best_fit);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    std::cout << "09.09 : ok\n";

    return 0;
}
