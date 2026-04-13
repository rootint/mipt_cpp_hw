/*
Реализуйте класс Tracer для представления трассировщика вызовов функций. Реализуйте в классе Tracer
пользовательский конструктор по умолчанию и пользовательский деструктор, которые будут выводить парные
сообщения. Используйте паттерн RAII. Предполагайте, что пользователь самостоятельно создает экземпляр
класса Tracer в начале каждой собственной функции. Используйте стандартный поток std::cout для вы-
вода через терминал всех сообщений. Используйте стандартную утилиту std::source_location для вывода
дополнительной информации в сообщениях. Реализуйте функциональный макрос trace по аналогии со стан-
дартным макросом assert, который позволит отключить всю трассировку при определении макроса NDEBUG.
*/

#include <iostream>
#include <source_location>
#include <stdexcept>
#include <string_view>

class Tracer
{
public:
    explicit Tracer(
        std::source_location location = std::source_location::current())
        : m_location(location), m_depth(s_depth)
    {
        ++s_depth;
        print("enter");
    }

    Tracer(Tracer const &) = delete;
    auto operator=(Tracer const &) -> Tracer & = delete;
    Tracer(Tracer &&) = delete;
    auto operator=(Tracer &&) -> Tracer & = delete;

    ~Tracer() noexcept
    {
        if (s_depth > 0)
        {
            --s_depth;
        }

        try
        {
            print("leave");
        }
        catch (...)
        {
        }
    }

private:
    static auto short_file_name(std::string_view path) -> std::string_view
    {
        auto const position = path.find_last_of("/\\");
        return position == std::string_view::npos
            ? path
            : path.substr(position + 1);
    }

    void print(char const *event) const
    {
        for (std::size_t i = 0; i < m_depth; ++i)
        {
            std::cout << "  ";
        }

        std::cout
            << event
            << " : "
            << m_location.function_name()
            << " ["
            << short_file_name(m_location.file_name())
            << ':'
            << m_location.line()
            << "]\n";
    }

    inline static thread_local std::size_t s_depth = 0;

    std::source_location m_location;
    std::size_t m_depth;
};

#define TRACE_CONCAT_IMPL(lhs, rhs) lhs##rhs
#define TRACE_CONCAT(lhs, rhs) TRACE_CONCAT_IMPL(lhs, rhs)

#ifndef NDEBUG
#define trace() [[maybe_unused]] Tracer TRACE_CONCAT(tracer_, __LINE__) {}
#else
#define trace() static_cast<void>(0)
#endif

auto multiply(int x, int y) -> int
{
    trace();
    return x * y;
}

auto compute(int value) -> int
{
    trace();
    return multiply(value, value) + multiply(value, 2);
}

void fail()
{
    trace();
    throw std::runtime_error("demo exception");
}

int main()
{
    trace();

    auto const result = compute(6);
    std::cout << "result = " << result << '\n';

    try
    {
        fail();
    }
    catch (std::runtime_error const &error)
    {
        std::cout << "caught : " << error.what() << '\n';
    }

    return 0;
}