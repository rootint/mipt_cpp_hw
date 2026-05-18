/*
Доработайте пример 13.18 таким образом, чтобы реализация использовала алгоритм условного отображения
только тех вхождений в директорию, названия которых соответствуют заданному пользователем регулярному
выражению, вместо безусловного отображения всех вхождений. Используйте стандартное регулярное выраже-
ние std::regex для реализации алгоритма поиска соответствий. Сравните данное решение с утилитой grep.
*/

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

auto make_type(std::filesystem::file_status const &status) -> char
{
    if (std::filesystem::is_directory(status)) { return 'd'; }
    if (std::filesystem::is_regular_file(status)) { return 'f'; }
    if (std::filesystem::is_symlink(status)) { return 'l'; }
    return '?';
}

auto make_permissions(std::filesystem::perms permissions) -> std::string
{
    auto lambda = [permissions](auto flag, char letter)
    {
        return (permissions & flag) == std::filesystem::perms::none ? '-' : letter;
    };

    return {
        lambda(std::filesystem::perms::owner_read, 'r'),
        lambda(std::filesystem::perms::owner_write, 'w'),
        lambda(std::filesystem::perms::owner_exec, 'x')};
}

auto size_of(std::filesystem::path const &path) -> std::size_t
{
    std::size_t total = 0;

    if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
    {
        for (auto const &entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (!std::filesystem::is_directory(entry.status()))
            {
                total += std::filesystem::file_size(entry);
            }
        }
    }

    return total;
}

auto formatted_size(std::filesystem::directory_entry const &entry) -> std::string
{
    std::size_t size = std::filesystem::is_regular_file(entry.status())
        ? std::filesystem::file_size(entry)
        : size_of(entry.path());

    std::vector<char> const units = {'B', 'K', 'M', 'G'};

    std::size_t index = 0;

    while (index + 1 < units.size() && size >= (1ull << 10))
    {
        size /= (1ull << 10);
        ++index;
    }

    return (std::stringstream() << std::format("{: >4} ({})", size, units[index])).str();
}

void show_filtered(std::filesystem::path const &path, std::regex const &pattern)
{
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
    {
        return;
    }

    for (auto const &entry : std::filesystem::directory_iterator(path))
    {
        auto const name = entry.path().filename().string();

        if (!std::regex_search(name, pattern))
        {
            continue;
        }

        std::cout << std::format(
            "show : entry : {} | {} | {} | {} | {}\n",
            make_type(entry.status()),
            make_permissions(entry.status().permissions()),
            formatted_size(entry),
            std::chrono::floor<std::chrono::seconds>(
                std::chrono::file_clock::to_sys(entry.last_write_time())),
            name);
    }
}

int main(int argc, char *argv[])
{
    auto const directory = std::filesystem::path(argc > 1 ? argv[1] : ".");
    auto const expression = std::string(argc > 2 ? argv[2] : R"(.*\.cpp$)");

    try
    {
        std::regex pattern(expression, std::regex::ECMAScript);
        std::cout << "directory : " << directory
                  << "\nregex     : " << expression << "\n\n";
        show_filtered(directory, pattern);
    }
    catch (std::regex_error const &error)
    {
        std::cerr << "invalid regex : " << error.what() << '\n';
        return 1;
    }

    return 0;
}
