/*
Доработайте Ваше предыдущее решение задачи 12.05 таким образом, чтобы реализация использовала фай-
ловый поток ввода для чтения инструкций и их результатов из файла вместо взаимодействия с пользователем
через терминал. Используйте стандартный поток std::fstream в режиме ввода для чтения тестовых данных.
*/

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

class Calculator
{
public:
    explicit Calculator(std::istream &stream) : m_stream(stream) {}

    auto expression() -> double
    {
        auto left = term();

        while (true)
        {
            auto const token = next_token();

            if (token == '+')
            {
                left += term();
            }
            else if (token == '-')
            {
                left -= term();
            }
            else
            {
                putback(token);
                return left;
            }
        }
    }

private:
    auto term() -> double
    {
        auto left = power();

        while (true)
        {
            auto const token = next_token();

            if (token == '*')
            {
                left *= power();
            }
            else if (token == '/')
            {
                auto const right = power();
                if (right == 0.0) { throw std::runtime_error("division by zero"); }
                left /= right;
            }
            else if (token == '%')
            {
                auto const right = power();
                if (right == 0.0) { throw std::runtime_error("modulo by zero"); }
                left = std::fmod(left, right);
            }
            else
            {
                putback(token);
                return left;
            }
        }
    }

    auto power() -> double
    {
        auto const left = factorial();

        auto const token = next_token();

        if (token == '^')
        {
            return std::pow(left, power());
        }

        putback(token);
        return left;
    }

    auto factorial() -> double
    {
        auto value = unary();

        while (true)
        {
            auto const token = next_token();

            if (token == '!')
            {
                value = std::tgamma(value + 1.0);
            }
            else
            {
                putback(token);
                return value;
            }
        }
    }

    auto unary() -> double
    {
        auto const token = next_token();

        if (token == '+') { return unary(); }
        if (token == '-') { return -unary(); }

        putback(token);
        return primary();
    }

    auto primary() -> double
    {
        auto const token = next_token();

        if (token == '(' || token == '[' || token == '{')
        {
            auto const value = expression();
            auto const close = next_token();

            auto const expected =
                token == '(' ? ')' :
                token == '[' ? ']' : '}';

            if (close != expected)
            {
                throw std::runtime_error("unbalanced brackets");
            }

            return value;
        }

        if (token == 'n')
        {
            return m_number;
        }

        throw std::runtime_error("primary expected");
    }

    auto next_token() -> char
    {
        if (m_has_putback)
        {
            m_has_putback = false;
            return m_putback;
        }

        char character = 0;

        if (!(m_stream >> character))
        {
            return 0;
        }

        switch (character)
        {
            case '+': case '-': case '*': case '/': case '%':
            case '^': case '!': case '(': case ')':
            case '[': case ']': case '{': case '}':
                return character;

            default:
                m_stream.putback(character);
                m_stream >> m_number;
                return 'n';
        }
    }

    void putback(char token)
    {
        m_putback = token;
        m_has_putback = true;
    }

    std::istream &m_stream;
    double m_number = 0.0;
    char m_putback = 0;
    bool m_has_putback = false;
};

auto evaluate(std::string const &source) -> double
{
    std::stringstream stream(source);
    Calculator calculator(stream);
    return calculator.expression();
}

auto run_file(std::string const &path) -> int
{
    std::fstream stream(path, std::ios::in);

    if (!stream)
    {
        std::cerr << "cannot open file : " << path << '\n';
        return 1;
    }

    int total = 0;
    int passed = 0;

    std::string line;

    while (std::getline(stream, line))
    {
        if (line.empty() || line.front() == '#')
        {
            continue;
        }

        auto const equals = line.find('=');

        if (equals == std::string::npos)
        {
            std::cerr << "skipping malformed line : " << line << '\n';
            continue;
        }

        auto const expression_text = line.substr(0, equals);
        auto const expected_text = line.substr(equals + 1);

        double expected = 0.0;
        std::stringstream(expected_text) >> expected;

        ++total;

        try
        {
            auto const actual = evaluate(expression_text);
            auto const ok = std::fabs(actual - expected) < 1e-9;

            std::cout << (ok ? "[pass] " : "[fail] ")
                      << expression_text << " = " << actual
                      << " (expected " << expected << ")\n";

            if (ok) { ++passed; }
        }
        catch (std::exception const &error)
        {
            std::cout << "[error] " << expression_text
                      << " -> " << error.what() << '\n';
        }
    }

    std::cout << "summary : " << passed << " / " << total << '\n';
    return passed == total ? 0 : 1;
}

int main(int argc, char *argv[])
{
    std::string const path = (argc > 1) ? argv[1] : "tests.txt";

    if (argc <= 1 && !std::filesystem::exists(path))
    {
        std::fstream stream(path, std::ios::out);
        stream << "# arithmetic expressions and expected values\n"
                  "2 + 3 * 4 = 14\n"
                  "(2 + 3) * 4 = 20\n"
                  "[2 + 3] * {4 - 1} = 15\n"
                  "7 % 4 = 3\n"
                  "2 ^ 10 = 1024\n"
                  "2 ^ 3 ^ 2 = 512\n"
                  "5! = 120\n"
                  "(2 + 1)! = 6\n"
                  "-3 + 5 = 2\n";
    }

    auto const result = run_file(path);

    if (argc <= 1)
    {
        std::filesystem::remove(path);
    }

    return result;
}
