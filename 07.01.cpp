/*
Реализуйте алгоритм вычисления корней алгебраического уравнения второй степени с коэффициентами a, b
и c типа double. Используйте ветвления if для проверки значения коэффициента a и значения дискриминан-
та. Используйте константу epsilon и стандартную функцию std::abs для корректного сравнения чисел типа
double с заданной точностью. Допускайте появление отрицательного нуля. Используйте стандартный сим-
вольный поток ввода std::cin для ввода коэффициентов a, b и c. Используйте стандартный символьный по-
ток вывода std::cout для вывода корней уравнения. Не сопровождайте Ваше решение данной задачи тестами.
*/
#include <iostream>
#include <cmath>
#include <optional>
#include <variant>

using Roots = std::variant<double, std::pair<double, double>, std::monostate>;

std::optional<Roots> solve(double a, double b, double c) {
    const double epsilon = 1e-9;

    if (std::abs(a) < epsilon) {
        if (std::abs(b) < epsilon) {
            if (std::abs(c) < epsilon) {
                return Roots{std::monostate{}};
            } else {
                return std::nullopt;
            }
        } else {
            return Roots{-c / b};
        }
    } else {
        double d = b * b - 4 * a * c;

        if (d < 0) {
            return std::nullopt;
        } else if (std::abs(d) < epsilon) {
            return Roots{-b / (2 * a)};
        } else {
            double sqrt_d = std::sqrt(d);
            double x1 = (-b + sqrt_d) / (2 * a);
            double x2 = (-b - sqrt_d) / (2 * a);
            return Roots{std::pair{x1, x2}};
        }
    }
}

int main() {
    double a, b, c;
    std::cin >> a >> b >> c;

    std::cout << a << "x^2 + " << b << "x + " << c << " = 0" << std::endl;

    auto result = solve(a, b, c);

    if (!result) {
        std::cout << "No solutions" << std::endl;
    } else if (std::holds_alternative<std::monostate>(*result)) {
        std::cout << "Infinite solutions" << std::endl;
    } else if (std::holds_alternative<double>(*result)) {
        std::cout << "Single solution: " << std::get<double>(*result) << std::endl;
    } else {
        auto [x1, x2] = std::get<std::pair<double, double>>(*result);
        std::cout << "Two solutions: " << x1 << " and " << x2 << std::endl;
    }

    return 0;
}

/* 
Tests output:
root@44051:~/mipt_cpp_hw# ./a.out 
1 2 3
1x^2 + 2x + 3 = 0
No solutions
root@44051:~/mipt_cpp_hw# ./a.out 
1 3 4
1x^2 + 3x + 4 = 0
No solutions
root@44051:~/mipt_cpp_hw# ./a.out 
1 -3 4
1x^2 + -3x + 4 = 0
No solutions
root@44051:~/mipt_cpp_hw# ./a.out 
1 3 -4
1x^2 + 3x + -4 = 0
Two solutions: 1 and -4
root@44051:~/mipt_cpp_hw# 
*/