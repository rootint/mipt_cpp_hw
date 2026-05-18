/*
Доработайте Ваше предыдущее решение задачи 07.01 таким образом, чтобы реализация использовала паттерн
Visitor для извлечения корней из варианта вместо ручной распаковки варианта. Реализуйте класс Visitor для
представления посетителя. Реализуйте в классе Visitor три перегруженных оператора вызова для обработки
каждого из возможных значений варианта. Используйте стандартную функцию std::visit для применения
перегруженных операторов вызова экземпляра класса Visitor к варианту, который был получен из опционала.
*/

#include <cmath>
#include <iostream>
#include <optional>
#include <utility>
#include <variant>

using roots_t = std::variant<std::monostate, double, std::pair<double, double>>;

auto solve(double a, double b, double c) -> std::optional<roots_t>
{
    if (a == 0.0)
    {
        if (b == 0.0)
        {
            if (c == 0.0)
            {
                return roots_t{std::monostate{}};
            }

            return std::nullopt;
        }

        return roots_t{-c / b};
    }

    auto const discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0)
    {
        return std::nullopt;
    }

    if (discriminant == 0.0)
    {
        return roots_t{-b / (2.0 * a)};
    }

    auto const sqrt_d = std::sqrt(discriminant);

    return roots_t{std::pair<double, double>{
        (-b - sqrt_d) / (2.0 * a),
        (-b + sqrt_d) / (2.0 * a)}};
}

class Visitor
{
public:
    void operator()(std::monostate) const
    {
        std::cout << "infinite roots\n";
    }

    void operator()(double root) const
    {
        std::cout << "one root : " << root << '\n';
    }

    void operator()(std::pair<double, double> const &roots) const
    {
        std::cout << "two roots : " << roots.first << ", " << roots.second << '\n';
    }
};

void show(double a, double b, double c)
{
    std::cout << a << " * x^2 + " << b << " * x + " << c << " = 0 -> ";

    auto const result = solve(a, b, c);

    if (!result)
    {
        std::cout << "no roots\n";
        return;
    }

    std::visit(Visitor{}, *result);
}

int main()
{
    show(1.0, -3.0, 2.0);
    show(1.0, -2.0, 1.0);
    show(1.0, 0.0, 1.0);
    show(0.0, 2.0, -4.0);
    show(0.0, 0.0, 5.0);
    show(0.0, 0.0, 0.0);

    return 0;
}
