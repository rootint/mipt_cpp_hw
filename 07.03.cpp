/*
Исследуйте представленную ниже функцию. Определите в ней такие элементы, которые могут приводить к
нормальному ветвлению пути выполнения или генерации некоторого исключения. Считайте, что Person явля-
ется пользовательским классом, а Status является пользовательским перечислением с областью видимости.
void test ( Person const & person )
{
std :: cout << " test : " << person . name () << ’\ n ’;
if ( person . grade () == 10 || person . salary () > 1 ’ 000 ’ 000)
{
save ( Status :: success , person . id () ) ;
}
else
{
save ( Status :: failure , person . id () ) ;
}
}
*/

#include <iostream>
#include <string>

void test(Person const &person)
{
    // потенциальные исключения:
    // может быть переопределен << для .name(), возможно исключение
    // person.name() может вызвать исключение
    std::cout << " test : " << person.name() << '\n';
    // if-else - нормальное ветвление;
    // если person.grade() == 10, то salary проверяться уже не будет
    // потенциальные исключения:
    // person.grade() и person.salary() могут вызвать исключения
    // если .grade() или .salary() возвращают не int, а операторы (== и >) не перегружены, возможно исключение
    if (person.grade() == 10 || person.salary() > 1000000)
    {
        // person.id() может вызвать исключение
        // save - тоже может вызвать исключение
        save(Status::success, person.id());
    }
    else
    {
        // person.id() может вызвать исключение
        // save - тоже может вызвать исключение
        save(Status::failure, person.id());
    }
}
// дополнительно, если криво прописаны деструкторы, там тоже может быть исключение