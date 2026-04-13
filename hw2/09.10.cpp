/*
Доработайте примеры 09.28, 09.29, 09.30 и 09.31 таким образом, чтобы реализации использовали наследование
интерфейсов от общего абстрактного базового класса вместо дублирования интерфейсов в отдельных
самостоятельных классах. Реализуйте абстрактный базовый класс Allocator для представления интерфейса
аллокаторов. Реализуйте в классе Allocator виртуальный деструктор и публичные чисто виртуальные функции-члены
allocate и deallocate для выделения и освобождения памяти соответственно. Реализуйте в классе Allocator
защищенный шаблон функции-члена get для получения указателя на объект стандартного типа std::byte или
пользовательских типов Header или Node. Реализуйте четыре производных класса аллокаторов для представления
различных методов выделения и освобождения памяти, которые являются наследниками интерфейса класса Allocator.
Продемонстрируйте использование полиморфных аллокаторов.
*/

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <new>
#include <string_view>
#include <utility>
#include <vector>

class Allocator
{
public:
    virtual ~Allocator() = default;

    virtual auto allocate(std::size_t size) -> void * = 0;
    virtual void deallocate(void *pointer) = 0;

protected:
    template <typename T>
    auto get(void *pointer) const -> T *
    {
        return static_cast<T *>(pointer);
    }

    template <typename T>
    auto get(void const *pointer) const -> T const *
    {
        return static_cast<T const *>(pointer);
    }
};

class LinearAllocator : public Allocator
{
public:
    explicit LinearAllocator(std::size_t size) : m_size(size)
    {
        m_begin = ::operator new(m_size, std::align_val_t(s_alignment));
    }

    ~LinearAllocator() override
    {
        ::operator delete(m_begin, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void * override
    {
        void *begin = get<std::byte>(m_begin) + m_offset;
        auto free = m_size - m_offset;

        if ((begin = std::align(s_alignment, size, begin, free)))
        {
            m_offset = m_size - free + size;
            return begin;
        }

        return nullptr;
    }

    void deallocate(void *) override
    {
    }

private:
    std::size_t m_size = 0;
    std::size_t m_offset = 0;
    void *m_begin = nullptr;

    static inline constexpr auto s_alignment = alignof(std::max_align_t);
};

class StackAllocator : public Allocator
{
public:
    explicit StackAllocator(std::size_t size) : m_size(size)
    {
        m_begin = ::operator new(m_size, std::align_val_t(s_alignment));
    }

    ~StackAllocator() override
    {
        ::operator delete(m_begin, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void * override
    {
        void *begin = get<std::byte>(m_begin) + m_offset + sizeof(Header);
        auto free = m_size - m_offset - sizeof(Header);

        if ((begin = std::align(s_alignment, size, begin, free)))
        {
            auto *header = get<Header>(get<std::byte>(begin) - sizeof(Header));
            header->offset = static_cast<std::size_t>(
                get<std::byte>(begin) - get<std::byte>(m_begin));
            header->previous_offset = m_offset;

            m_offset = header->offset + size;
            return begin;
        }

        return nullptr;
    }

    void deallocate(void *pointer) override
    {
        if (!pointer)
        {
            return;
        }

        auto *header = get<Header>(get<std::byte>(pointer) - sizeof(Header));
        m_offset = header->previous_offset;
    }

private:
    struct Header
    {
        std::size_t previous_offset = 0;
        std::size_t offset = 0;
    };

    std::size_t m_size = 0;
    std::size_t m_offset = 0;
    void *m_begin = nullptr;

    static inline constexpr auto s_alignment = alignof(std::max_align_t);
};

class PoolAllocator : public Allocator
{
public:
    PoolAllocator(std::size_t size, std::size_t step)
        : m_size(size), m_step(step)
    {
        assert(m_size % m_step == 0);
        assert(m_step >= sizeof(Node));

        make_chunk();
    }

    ~PoolAllocator() override
    {
        for (auto *chunk : m_chunks)
        {
            ::operator delete(chunk, std::align_val_t(s_alignment));
        }
    }

    auto allocate(std::size_t size) -> void * override
    {
        if (size > m_step)
        {
            return nullptr;
        }

        if (!m_head)
        {
            make_chunk();
        }

        auto *node = m_head;

        if (node->next)
        {
            m_head = node->next;
        }
        else
        {
            auto *next = get<std::byte>(node) + m_step;

            if (next != get<std::byte>(m_chunks.back()) + m_size)
            {
                m_head = get<Node>(next);
                m_head->next = nullptr;
            }
            else
            {
                m_head = nullptr;
            }
        }

        return node;
    }

    void deallocate(void *pointer) override
    {
        if (!pointer)
        {
            return;
        }

        auto *node = get<Node>(pointer);
        node->next = m_head;
        m_head = node;
    }

private:
    struct Node
    {
        Node *next = nullptr;
    };

    void make_chunk()
    {
        auto *chunk = ::operator new(m_size, std::align_val_t(s_alignment));
        m_chunks.push_back(chunk);
        m_head = get<Node>(chunk);
        m_head->next = nullptr;
    }

    std::size_t m_size = 0;
    std::size_t m_step = 0;
    Node *m_head = nullptr;
    std::vector<void *> m_chunks;

    static inline constexpr auto s_alignment = alignof(std::max_align_t);
};

class FreeListAllocator : public Allocator
{
public:
    explicit FreeListAllocator(std::size_t size) : m_size(size)
    {
        assert(m_size >= sizeof(Node) + 1);

        m_begin = ::operator new(m_size, std::align_val_t(s_alignment));
        m_head = get<Node>(m_begin);
        m_head->size = m_size - sizeof(Header);
        m_head->next = nullptr;
    }

    ~FreeListAllocator() override
    {
        ::operator delete(m_begin, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void * override
    {
        void *end = get<std::byte>(m_begin) + sizeof(Header) + size;
        void *next = end;
        auto free = 2 * alignof(Header);

        if ((next = std::align(alignof(Header), sizeof(Header), next, free)))
        {
            auto padding = static_cast<std::size_t>(
                get<std::byte>(next) - get<std::byte>(end));

            auto [current, previous] = find(size + padding);

            if (!current)
            {
                return nullptr;
            }

            if (current->size >= size + padding + sizeof(Node) + 1)
            {
                auto const step = sizeof(Header) + size + padding;
                auto *node = get<Node>(get<std::byte>(current) + step);
                node->size = current->size - step;
                node->next = current->next;
                current->next = node;
            }
            else
            {
                padding += current->size - size - padding;
            }

            if (!previous)
            {
                m_head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            auto *header = get<Header>(current);
            header->size = size + padding;

            return get<std::byte>(current) + sizeof(Header);
        }

        return nullptr;
    }

    void deallocate(void *pointer) override
    {
        if (!pointer)
        {
            return;
        }

        auto *node = get<Node>(get<std::byte>(pointer) - sizeof(Header));
        auto *header = get<Header>(node);

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

    auto find(std::size_t size) const -> std::pair<Node *, Node *>
    {
        Node *current = m_head;
        Node *previous = nullptr;

        while (current && current->size < size)
        {
            previous = current;
            current = current->next;
        }

        return {current, previous};
    }

    void merge(Node *previous, Node *node)
    {
        if (node->next &&
            get<std::byte>(node) + sizeof(Header) + node->size ==
                get<std::byte>(node->next))
        {
            node->size += sizeof(Header) + node->next->size;
            node->next = node->next->next;
        }

        if (previous &&
            get<std::byte>(previous) + sizeof(Header) + previous->size ==
                get<std::byte>(node))
        {
            previous->size += sizeof(Header) + node->size;
            previous->next = node->next;
        }
    }

    std::size_t m_size = 0;
    void *m_begin = nullptr;
    Node *m_head = nullptr;

    static inline constexpr auto s_alignment = alignof(std::max_align_t);
};

void run_polymorphic_case(
    std::string_view name,
    Allocator &allocator,
    std::vector<std::size_t> const &sizes)
{
    std::vector<void *> blocks;

    for (auto size : sizes)
    {
        auto *pointer = allocator.allocate(size);
        assert(pointer != nullptr);
        blocks.push_back(pointer);
    }

    for (auto it = blocks.rbegin(); it != blocks.rend(); ++it)
    {
        allocator.deallocate(*it);
    }

    std::cout << "polymorphic case : " << name << '\n';
}

int main()
{
    LinearAllocator linear(256);
    auto *linear_a = linear.allocate(16);
    auto *linear_b = linear.allocate(24);
    assert(linear_a != nullptr);
    assert(linear_b != nullptr);
    assert(linear_a != linear_b);

    StackAllocator stack(256);
    auto *stack_a = stack.allocate(16);
    auto *stack_b = stack.allocate(24);
    stack.deallocate(stack_b);
    auto *stack_c = stack.allocate(24);
    assert(stack_c == stack_b);
    stack.deallocate(stack_c);
    stack.deallocate(stack_a);

    PoolAllocator pool(64, 16);
    auto *pool_a = pool.allocate(16);
    auto *pool_b = pool.allocate(16);
    pool.deallocate(pool_a);
    auto *pool_c = pool.allocate(16);
    assert(pool_c == pool_a);
    pool.deallocate(pool_b);
    pool.deallocate(pool_c);

    FreeListAllocator free_list(1024);
    auto *free_a = free_list.allocate(16);
    auto *free_b = free_list.allocate(16);
    free_list.deallocate(free_b);
    free_list.deallocate(free_a);
    auto *free_c = free_list.allocate(32);
    assert(free_c == free_a);
    free_list.deallocate(free_c);

    std::vector<std::unique_ptr<Allocator>> allocators;
    allocators.push_back(std::make_unique<LinearAllocator>(256));
    allocators.push_back(std::make_unique<StackAllocator>(256));
    allocators.push_back(std::make_unique<PoolAllocator>(128, 16));
    allocators.push_back(std::make_unique<FreeListAllocator>(1024));

    run_polymorphic_case("linear", *allocators[0], {16, 16, 16});
    run_polymorphic_case("stack", *allocators[1], {16, 24, 32});
    run_polymorphic_case("pool", *allocators[2], {16, 16, 16});
    run_polymorphic_case("free_list", *allocators[3], {16, 24, 32});

    std::cout << "09.10 : ok\n";

    return 0;
}
