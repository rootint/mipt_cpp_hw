/*
Доработайте пример 13.15 таким образом, чтобы реализация использовала алгоритм удаления пустых строк
и строк, состоящих из пробельных символов, вместо сохранения их в итоговом файле. Реализуйте обработку
сырых строковых литералов в алгоритме. Рассмотрите особенности оформления сырых строковых литералов.
*/

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>

auto read_file(std::string const &path) -> std::string
{
    std::fstream stream(path, std::ios::in);
    return (std::stringstream() << stream.rdbuf()).str();
}

void write_file(std::string const &path, std::string_view content)
{
    std::fstream(path, std::ios::out) << content;
}

auto strip_raw_delimiter(std::string const &text, std::size_t position) -> std::string
{
    std::string delimiter;

    for (auto i = position + 2; i < text.size() && text[i] != '('; ++i)
    {
        delimiter.push_back(text[i]);
    }

    return ")" + delimiter + "\"";
}

auto strip_comments(std::string text) -> std::string
{
    std::string output;
    output.reserve(text.size());

    for (std::size_t i = 0; i < text.size();)
    {
        auto const character = text[i];

        if (character == 'R' && i + 1 < text.size() && text[i + 1] == '"')
        {
            auto const terminator = strip_raw_delimiter(text, i);

            auto const start = text.find('(', i + 2);

            if (start == std::string::npos)
            {
                output.push_back(character);
                ++i;
                continue;
            }

            auto const end = text.find(terminator, start + 1);

            if (end == std::string::npos)
            {
                output.append(text, i, std::string::npos);
                break;
            }

            auto const last = end + terminator.size();
            output.append(text, i, last - i);
            i = last;
            continue;
        }

        if (character == '\'' || character == '"')
        {
            output.push_back(character);
            ++i;

            while (i < text.size())
            {
                output.push_back(text[i]);

                if (text[i] == '\\' && i + 1 < text.size())
                {
                    output.push_back(text[i + 1]);
                    i += 2;
                    continue;
                }

                if (text[i] == character)
                {
                    ++i;
                    break;
                }

                ++i;
            }

            continue;
        }

        if (character == '/' && i + 1 < text.size())
        {
            if (text[i + 1] == '/')
            {
                i += 2;

                while (i < text.size() && text[i] != '\n')
                {
                    ++i;
                }

                continue;
            }

            if (text[i + 1] == '*')
            {
                i += 2;

                while (i + 1 < text.size() && !(text[i] == '*' && text[i + 1] == '/'))
                {
                    ++i;
                }

                i = std::min(i + 2, text.size());
                continue;
            }
        }

        output.push_back(character);
        ++i;
    }

    return output;
}

auto drop_blank_lines(std::string const &text) -> std::string
{
    std::string output;
    output.reserve(text.size());

    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line))
    {
        auto const blank = std::all_of(
            std::cbegin(line), std::cend(line),
            [](unsigned char character) { return std::isspace(character); });

        if (!blank)
        {
            output.append(line);
            output.push_back('\n');
        }
    }

    return output;
}

void transform(std::string const &source_path, std::string const &target_path)
{
    auto const original = read_file(source_path);
    auto const stripped = strip_comments(original);
    auto const compact = drop_blank_lines(stripped);
    write_file(target_path, compact);
}

int main()
{
    auto const source_path = std::string("source.cpp");
    auto const target_path = std::string("output.cpp");

    write_file(source_path, R"SRC(// leading line comment
#include <iostream>

/* block comment
   spanning lines */

int main()
{
    auto raw = R"(hello /* not a comment */)";

    auto line = "string with // not a comment";

    /* trailing */ std::cout << raw << '\n';
    
    return 0;   // tail comment
}
)SRC");

    transform(source_path, target_path);

    std::cout << "=== transformed ===\n" << read_file(target_path);

    std::filesystem::remove(source_path);
    std::filesystem::remove(target_path);

    return 0;
}
