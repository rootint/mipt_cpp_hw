/*
Реализуйте алгоритм преобразования коллекции восьмибитных беззнаковых целых чисел в строку с их шест-
надцатиричными представлениями. Используйте стандартный контейнер std::vector с элементами стан-
дартного типа std::uint8_t для хранения коллекции восьмибитных беззнаковых целых чисел. Используйте
стандартный поток std::stringstream для создания необходимой строки. Используйте стандартные мани-
пуляторы std::setw, right, std::setfill и std::hex для форматирования вывода. Реализуйте обратный
алгоритм преобразования строки, состоящей из четного количества шестнадцатиричных цифр, в коллекцию
восьмибитных беззнаковых целых чисел. Используйте вычитание символов и побитовые операции для реализа-
ции обратного алгоритма преобразования. Используйте только шестнадцатиричные цифры в нижнем регистре.
*/

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

auto to_hex(std::vector<std::uint8_t> const &bytes) -> std::string
{
    std::stringstream stream;
    stream << std::hex << std::setfill('0');

    for (auto const byte : bytes)
    {
        stream << std::setw(2) << std::right << static_cast<unsigned int>(byte);
    }

    return stream.str();
}

auto from_hex_digit(char digit) -> std::uint8_t
{
    if (digit >= '0' && digit <= '9')
    {
        return static_cast<std::uint8_t>(digit - '0');
    }

    if (digit >= 'a' && digit <= 'f')
    {
        return static_cast<std::uint8_t>(digit - 'a' + 10);
    }

    throw std::invalid_argument("invalid hex digit");
}

auto from_hex(std::string_view text) -> std::vector<std::uint8_t>
{
    if (text.size() % 2 != 0)
    {
        throw std::invalid_argument("odd number of hex digits");
    }

    std::vector<std::uint8_t> bytes;
    bytes.reserve(text.size() / 2);

    for (std::size_t i = 0; i < text.size(); i += 2)
    {
        auto const high = from_hex_digit(text[i]);
        auto const low = from_hex_digit(text[i + 1]);
        bytes.push_back(static_cast<std::uint8_t>((high << 4) | low));
    }

    return bytes;
}

int main()
{
    std::vector<std::uint8_t> const bytes = {0x00, 0x0f, 0x10, 0xab, 0xcd, 0xef, 0xff};

    auto const encoded = to_hex(bytes);

    std::cout << "encoded : " << encoded << '\n';

    assert(encoded == "000f10abcdefff");

    auto const decoded = from_hex(encoded);

    assert(decoded == bytes);

    std::cout << "decoded : ";

    for (auto const byte : decoded)
    {
        std::cout << static_cast<int>(byte) << ' ';
    }

    std::cout << '\n';

    return 0;
}
