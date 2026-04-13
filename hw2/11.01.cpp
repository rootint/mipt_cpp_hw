/*
Реализуйте функцию, которая возвращает указатель на себя таким образом, чтобы следующий код работал:
Wrapper function = test(); (*function)(); Реализуйте класс Wrapper для представления оболочки вокруг указателя
на функцию test. Реализуйте в классе Wrapper перегруженный оператор преобразования типа.
*/

#include <cassert>
#include <iostream>

class Wrapper
{
public:
    using function_t = Wrapper (*)();

    explicit Wrapper(function_t function = nullptr) : m_function(function) {}

    operator function_t() const
    {
        return m_function;
    }

private:
    function_t m_function = nullptr;
};

auto test() -> Wrapper
{
    return Wrapper(&test);
}

int main()
{
    Wrapper function = test();

    auto const self = (*function)();
    auto const pointer = static_cast<Wrapper::function_t>(self);

    assert(pointer == &test);

    std::cout << "11.01 : ok\n";

    return 0;
}
