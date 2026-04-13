/*
Доработайте пример 06.09 таким образом, чтобы реализация использовала повторно однократно выделенную
неинициализированную память для хранения деталей реализации основного класса вместо многократного выделения
и освобождения памяти. Обоснуйте накладные расходы и влияние на производительность оригинального паттерна
Pimpl. Реализуйте в классе Entity приватный стандартный контейнер std::array со спецификатором
alignas(std::max_align_t) и шестнадцатью элементами стандартного типа std::byte для хранения экземпляра
класса Implementation. Реализуйте в конструкторе класса Entity два статических утверждения static_assert для
проверки размера и выравнивания класса Implementation. Используйте размещающую версию оператора new в
конструкторе класса Entity. Используйте стандартную функцию std::destroy_at в деструкторе класса Entity.
Реализуйте в классе Entity константную и неконстантную публичные функции-члены get для получения отмытого
указателя на экземпляр класса Implementation. Используйте стандартные функции std::bit_cast и std::launder
в обеих функциях-членах get класса Entity.
*/

#include <array>
#include <cassert>
#include <bit>
#include <cstddef>
#include <iostream>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

class Entity
{
private:
    class Implementation;

public:
    Entity();

    Entity(Entity const &) = delete;
    auto operator=(Entity const &) -> Entity & = delete;

    Entity(Entity &&other);

    auto operator=(Entity &&other) -> Entity &;

    ~Entity();

    void set(int value, double factor);

    auto evaluate() const -> double;

    void test() const;

    auto get() -> Implementation *;

    auto get() const -> Implementation const *;

    static constexpr std::size_t storage_size = 16;

    alignas(std::max_align_t) std::array<std::byte, storage_size> m_storage{};
};

class Entity::Implementation
{
public:
    Implementation(int value, double factor)
        : value(value), factor(factor)
    {
    }

    auto evaluate() const
    {
        return value * factor;
    }

    int value = 0;
    double factor = 0.0;
};

Entity::Entity()
{
    static_assert(sizeof(Implementation) <= storage_size);
    static_assert(alignof(Implementation) <= alignof(std::max_align_t));

    ::new (m_storage.data()) Implementation(42, 1.5);
}

Entity::Entity(Entity &&other)
{
    static_assert(sizeof(Implementation) <= storage_size);
    static_assert(alignof(Implementation) <= alignof(std::max_align_t));

    ::new (m_storage.data()) Implementation(std::move(*other.get()));
}

auto Entity::operator=(Entity &&other) -> Entity &
{
    if (this != &other)
    {
        *get() = std::move(*other.get());
    }

    return *this;
}

Entity::~Entity()
{
    std::destroy_at(get());
}

void Entity::set(int value, double factor)
{
    get()->value = value;
    get()->factor = factor;
}

auto Entity::evaluate() const -> double
{
    return get()->evaluate();
}

void Entity::test() const
{
    std::cout
        << "value = " << get()->value
        << ", factor = " << get()->factor
        << ", result = " << get()->evaluate()
        << '\n';
}

auto Entity::get() -> Implementation *
{
    auto pointer = std::bit_cast<Implementation *>(m_storage.data());
    return std::launder(pointer);
}

auto Entity::get() const -> Implementation const *
{
    auto pointer = std::bit_cast<Implementation const *>(m_storage.data());
    return std::launder(pointer);
}

int main()
{
    static_assert(sizeof(Entity) >= 16);

    Entity first;
    first.test();

    assert(first.evaluate() == 63.0);

    auto const pointer = first.get();
    assert(pointer != nullptr);

    first.set(10, 2.0);

    Entity second = std::move(first);
    second.test();

    assert(second.evaluate() == 20.0);

    std::cout
        << "classic pimpl overhead: extra heap allocation, pointer indirection, "
           "and potentially worse cache locality.\n";
    std::cout
        << "inline storage avoids repeated allocate/free operations when the "
           "implementation fits in the fixed buffer.\n";
    std::cout << "09.07 : ok\n";

    return 0;
}
