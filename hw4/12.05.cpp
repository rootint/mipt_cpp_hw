/*
Доработайте примеры 12.19 и 12.26 таким образом, чтобы пользователь мог брать остаток от деления, воз-
водить в степень и вычислять факториал чисел, а также использовать квадратные и фигурные скобки. Реа-
лизуйте грамматический разбор и вычисление выражений на основе бинарного оператора процент для взятия
остатка от деления первого операнда на второй, бинарного оператора исключающее или для возведения пер-
вого операнда в степень второго и унарного постфиксного оператора восклицательный знак для вычисления
факториала операнда. Не рассматривайте проблемы переполнения и точности типа double при вычислении
факториала чисел. Реализуйте грамматический разбор и вычисление выражений на основе квадратных и фи-
гурных скобок. Используйте реализацию рекурсивного калькулятора Бьерна Страуструпа в качестве подсказки.
*/

#include <cctype>
#include <cmath>
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

                if (right == 0.0)
                {
                    throw std::runtime_error("division by zero");
                }

                left /= right;
            }
            else if (token == '%')
            {
                auto const right = power();

                if (right == 0.0)
                {
                    throw std::runtime_error("modulo by zero");
                }

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
        auto left = factorial();

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

        if (token == '+')
        {
            return unary();
        }

        if (token == '-')
        {
            return -unary();
        }

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
            case '[': case ']': case '{': case '}': case ';':
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

int main()
{
    struct Test { std::string expression; double expected; };

    Test const tests[] = {
        {"2 + 3 * 4", 14.0},
        {"(2 + 3) * 4", 20.0},
        {"[2 + 3] * {4 - 1}", 15.0},
        {"7 % 4", 3.0},
        {"2 ^ 10", 1024.0},
        {"2 ^ 3 ^ 2", 512.0},
        {"5!", 120.0},
        {"(2 + 1)!", 6.0},
        {"-3 + 5", 2.0},
        {"{[(1 + 2) * 3 + 4]^2 - 5} % 7", 3.0},
    };

    for (auto const &test : tests)
    {
        auto const value = evaluate(test.expression);
        std::cout << test.expression << " = " << value
                  << " (expected " << test.expected << ")\n";
    }

    return 0;
}
