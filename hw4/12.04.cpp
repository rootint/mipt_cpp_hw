/*
Доработайте пример 12.18 таким образом, чтобы пользователь мог извлечь все адреса электронных почт и
их домены из некоторого текста. Используйте упрощенный формат адресов электронных почт. Используйте
группу для извлечения доменов. Используйте сырые строковые литералы для реализации текстов для тестов.
*/

#include <iostream>
#include <regex>
#include <string>
#include <vector>

struct Email
{
    std::string address;
    std::string domain;
};

auto extract_emails(std::string const &text) -> std::vector<Email>
{
    std::regex pattern(R"(([A-Za-z0-9._%+-]+)@([A-Za-z0-9.-]+\.[A-Za-z]{2,}))");

    std::vector<Email> emails;

    auto begin = std::sregex_iterator(std::cbegin(text), std::cend(text), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        emails.push_back({(*it)[0].str(), (*it)[2].str()});
    }

    return emails;
}

int main()
{
    auto const text = std::string(R"(
        Please contact us at support@example.com for assistance.
        For business inquiries reach out to sales.team@company.co.uk
        or to ceo@startup.io. Do not reply to noreply@mailer.org.
        Bogus addresses like @nope.com and user@ are ignored.
    )");

    auto const emails = extract_emails(text);

    for (auto const &email : emails)
    {
        std::cout << "address : " << email.address
                  << " | domain : " << email.domain << '\n';
    }

    return 0;
}
