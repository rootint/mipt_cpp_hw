/*
Доработайте пример 09.23 таким образом, чтобы пользователь мог обслуживать память встроенных динамических
массивов с помощью перегруженных функций выделения и освобождения памяти. Реализуйте в классе Entity
публичные статические функции-члены operator new[] и operator delete[] для выделения и освобождения памяти
встроенных динамических массивов. Реализуйте в классе Entity публичные статические функции-члены operator new,
operator delete, operator new[] и operator delete[] для выделения и освобождения памяти встроенных
динамических массивов без генерации исключений, которые имеют дополнительный второй параметр стандартного
типа std::nothrow_t, передаваемый с помощью константной lvalue-ссылки. Реализуйте в классе Client все
необходимые дополнительные публичные объявления using.
*/

#include <cassert>
#include <cstddef>
#include <iostream>
#include <new>

template <typename D>
class Entity
{
public:
    static auto operator new(std::size_t size) -> void *
    {
        std::cout << "Entity::operator new\n";
        return ::operator new(size);
    }

    static void operator delete(void *pointer)
    {
        std::cout << "Entity::operator delete\n";
        ::operator delete(pointer);
    }

    static auto operator new[](std::size_t size) -> void *
    {
        std::cout << "Entity::operator new[]\n";
        return ::operator new[](size);
    }

    static void operator delete[](void *pointer)
    {
        std::cout << "Entity::operator delete[]\n";
        ::operator delete[](pointer);
    }

    static auto operator new(std::size_t size, std::nothrow_t const &) noexcept
        -> void *
    {
        std::cout << "Entity::operator new (nothrow)\n";
        return ::operator new(size, std::nothrow);
    }

    static void operator delete(
        void *pointer,
        std::nothrow_t const &) noexcept
    {
        std::cout << "Entity::operator delete (nothrow)\n";
        ::operator delete(pointer, std::nothrow);
    }

    static auto operator new[](
        std::size_t size,
        std::nothrow_t const &) noexcept -> void *
    {
        std::cout << "Entity::operator new[] (nothrow)\n";
        return ::operator new[](size, std::nothrow);
    }

    static void operator delete[](
        void *pointer,
        std::nothrow_t const &) noexcept
    {
        std::cout << "Entity::operator delete[] (nothrow)\n";
        ::operator delete[](pointer, std::nothrow);
    }

protected:
    Entity() = default;
};

class Client : private Entity<Client>
{
public:
    Client()
    {
        std::cout << "Client::Client\n";
    }

    ~Client()
    {
        std::cout << "Client::~Client\n";
    }

    using Entity<Client>::operator new;
    using Entity<Client>::operator delete;
    using Entity<Client>::operator new[];
    using Entity<Client>::operator delete[];
};

int main()
{
    auto *single = new Client;
    delete single;

    auto *array = new Client[2];
    delete[] array;

    auto *single_nothrow = new (std::nothrow) Client;
    assert(single_nothrow != nullptr);
    delete single_nothrow;

    auto *array_nothrow = new (std::nothrow) Client[3];
    assert(array_nothrow != nullptr);
    delete[] array_nothrow;

    auto *raw_single = Client::operator new(sizeof(Client), std::nothrow);
    assert(raw_single != nullptr);
    Client::operator delete(raw_single, std::nothrow);

    auto *raw_array = Client::operator new[](2 * sizeof(Client), std::nothrow);
    assert(raw_array != nullptr);
    Client::operator delete[](raw_array, std::nothrow);

    std::cout << "09.08 : ok\n";

    return 0;
}
