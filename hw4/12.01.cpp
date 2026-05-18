/*
Реализуйте алгоритм конвертации валюты RUB в валюту USD с поддержкой ввода и вывода локализованных
обозначений денежных сумм. Используйте стандартные локали std::locale для настройки потоков ввода
и вывода. Используйте русскую локаль ru_RU.utf8 для настройки потока ввода. Используйте американскую
локаль en_US.utf8 для настройки потока вывода. Используйте стандартные манипуляторы std::get_money
и std::put_money для ввода и вывода. Используйте стандартные потоки std::stringstream для реализации
потоков ввода и вывода. Указывайте обозначение валюты RUB до или после указания денежной суммы в RUB.
*/

#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

auto convert(std::string const &rub_input, double rate) -> std::string
{
    std::stringstream input_stream(rub_input);
    input_stream.imbue(std::locale("ru_RU.utf8"));

    long double amount_rub_kopecks = 0.0L;
    input_stream >> std::get_money(amount_rub_kopecks, true);

    auto const amount_usd_cents = (amount_rub_kopecks / rate);

    std::stringstream output_stream;
    output_stream.imbue(std::locale("en_US.utf8"));
    output_stream << std::showbase << std::put_money(amount_usd_cents, true);

    return output_stream.str();
}

int main()
{
    constexpr double rub_per_usd = 90.0;

    std::string const before = "RUB 9000,00";
    std::string const after = "9000,00 RUB";

    auto const usd_from_before = convert(before, rub_per_usd);
    auto const usd_from_after = convert(after, rub_per_usd);

    std::cout << before << " -> " << usd_from_before << '\n';
    std::cout << after << " -> " << usd_from_after << '\n';

    return 0;
}
